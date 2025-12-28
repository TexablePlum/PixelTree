import 'dart:async';
import 'package:dio/dio.dart';
import 'package:network_info_plus/network_info_plus.dart';
import '../models/wifi_network.dart';
import 'provisioning_service.dart';

// ============================================================================
// HTTP Provisioning Service
// ============================================================================
// Access Point + HTTP provisioning implementation

class HTTPProvisioningService implements ProvisioningService {
  final Dio _dio;
  final NetworkInfo _networkInfo;
  String? _baseUrl;

  ProvisioningState _state = ProvisioningState.idle;
  final _stateController = StreamController<ProvisioningState>.broadcast();

  HTTPProvisioningService({Dio? dio, NetworkInfo? networkInfo})
    : _dio = dio ?? Dio(),
      _networkInfo = networkInfo ?? NetworkInfo() {
    // Configure Dio
    _dio.options.connectTimeout = const Duration(seconds: 10);
    _dio.options.receiveTimeout = const Duration(seconds: 10);
  }

  @override
  ProvisioningState get state => _state;

  @override
  Stream<ProvisioningState> get stateStream => _stateController.stream;

  void _setState(ProvisioningState newState) {
    _state = newState;
    _stateController.add(newState);
  }

  // ========================================================================
  // Initialize (Verify AP Connection)
  // ========================================================================

  @override
  Future<void> initialize() async {
    try {
      _setState(ProvisioningState.initializing);

      // Check if connected to PixelTree AP
      final wifiName = await _networkInfo.getWifiName();

      if (wifiName == null || !wifiName.contains('PixelTree')) {
        throw Exception('Not connected to PixelTree Access Point');
      }

      // Set base URL (ESP32 AP default IP)
      _baseUrl = 'http://192.168.4.1';

      // Verify connectivity with status endpoint
      final response = await _dio.get('$_baseUrl/api/status');

      if (response.statusCode == 200) {
        _setState(ProvisioningState.connected);
      } else {
        throw Exception('Cannot communicate with PixelTree');
      }
    } catch (e) {
      _setState(ProvisioningState.failed);
      rethrow;
    }
  }

  // ========================================================================
  // Key Exchange (Not needed for HTTP)
  // ========================================================================

  @override
  Future<bool> performKeyExchange() async {
    // HTTP doesn't use encryption - credentials sent over local AP
    // This is acceptable since it's direct device-to-device
    return true;
  }

  // ========================================================================
  // WiFi Scanning
  // ========================================================================

  @override
  Future<List<WiFiNetwork>> scanNetworks() async {
    try {
      _setState(ProvisioningState.scanning);

      // 1. Trigger scan
      await _dio.post('$_baseUrl/api/scan');

      // 2. Wait for scan to complete
      await Future.delayed(const Duration(seconds: 3));

      // 3. Get scan results
      final response = await _dio.get('$_baseUrl/api/networks');

      if (response.statusCode == 200) {
        // Parse JSON array
        final List<dynamic> jsonList = response.data as List<dynamic>;
        final networks = jsonList
            .map((json) => WiFiNetwork.fromJson(json as Map<String, dynamic>))
            .toList();

        _setState(ProvisioningState.scanned);
        return networks;
      } else {
        throw Exception('Failed to get scan results');
      }
    } catch (e) {
      _setState(ProvisioningState.failed);
      rethrow;
    }
  }

  // ========================================================================
  // Provisioning
  // ========================================================================

  @override
  Future<bool> provisionDevice(
    String ssid,
    String password, {
    Duration timeout = const Duration(seconds: 45),
  }) async {
    try {
      _setState(ProvisioningState.sendingCredentials);

      // 1. Send credentials
      final response = await _dio.post(
        '$_baseUrl/api/provision',
        data: {'ssid': ssid, 'password': password},
      );

      if (response.statusCode != 200) {
        _setState(ProvisioningState.failed);
        return false;
      }

      // 2. Check immediate response
      final data = response.data as Map<String, dynamic>;
      if (data['status'] == 'success' && data['connected'] == true) {
        _setState(ProvisioningState.success);
        return true;
      }

      // 3. Poll for status (if not immediately successful)
      _setState(ProvisioningState.connecting);

      final endTime = DateTime.now().add(timeout);
      while (DateTime.now().isBefore(endTime)) {
        await Future.delayed(const Duration(seconds: 2));

        try {
          final statusResponse = await _dio.get('$_baseUrl/api/status');
          final statusData = statusResponse.data as Map<String, dynamic>;

          if (statusData['connected'] == true) {
            _setState(ProvisioningState.success);
            return true;
          }
        } catch (e) {
          // Connection might be lost during WiFi switch
          // This is expected behavior
          continue;
        }
      }

      // Timeout
      _setState(ProvisioningState.failed);
      return false;
    } catch (e) {
      _setState(ProvisioningState.failed);
      rethrow;
    }
  }

  // ========================================================================
  // Cleanup
  // ========================================================================

  @override
  Future<void> dispose() async {
    _dio.close();
    await _stateController.close();
  }
}
