import 'dart:async';
import '../../core/constants/ble_uuids.dart';
import '../datasources/ble_datasource.dart';
import '../datasources/crypto_datasource.dart';
import '../models/wifi_network.dart';
import '../models/pixeltree_device.dart';

// ============================================================================
// BLE Provisioning Repository
// ============================================================================
// High-level orchestration of BLE provisioning flow:
// 1. Connect to device
// 2. ECDH key exchange
// 3. WiFi network scanning
// 4. Credential submission (encrypted)
// 5. Status monitoring

class BLEProvisioningRepository {
  final BLEDataSource _bleDataSource;
  final ECDHCryptoService _cryptoService;

  ProvisioningState _state = ProvisioningState.idle;
  StreamSubscription<String>? _statusSubscription;

  BLEProvisioningRepository({
    required BLEDataSource bleDataSource,
    required ECDHCryptoService cryptoService,
  }) : _bleDataSource = bleDataSource,
       _cryptoService = cryptoService;

  // ========================================================================
  // State Management
  // =================================================================== =====

  ProvisioningState get state => _state;

  void _setState(ProvisioningState newState) {
    _state = newState;
  }

  // ========================================================================
  // Connection
  // ========================================================================

  /// Connect to device and prepare for provisioning
  Future<bool> connectToDevice(PixelTreeDevice device) async {
    try {
      _setState(ProvisioningState.connecting);

      await _bleDataSource.connectToDevice(device);

      _setState(ProvisioningState.connected);
      return true;
    } catch (e) {
      _setState(ProvisioningState.failed);
      rethrow;
    }
  }

  /// Disconnect from device
  Future<void> disconnect() async {
    await _statusSubscription?.cancel();
    _statusSubscription = null;
    await _bleDataSource.disconnect();
    _setState(ProvisioningState.idle);
  }

  // ========================================================================
  // Key Exchange (ECDH)
  // ========================================================================

  /// Perform ECDH key exchange with ESP32
  Future<bool> performKeyExchange() async {
    try {
      _setState(ProvisioningState.keyExchange);

      // 1. Generate our key pair
      final ourPublicKey = await _cryptoService.generateKeyPair();

      // 2. Read ESP32's public key
      final espPublicKey = await _bleDataSource.readCharacteristic(
        BLEUUIDs.keyExchangeService,
        BLEUUIDs.publicKeyESP,
      );

      // 3. Compute shared secret
      await _cryptoService.computeSharedSecret(espPublicKey);

      // 4. Send our public key to ESP32
      await _bleDataSource.writeCharacteristic(
        BLEUUIDs.keyExchangeService,
        BLEUUIDs.publicKeyApp,
        ourPublicKey,
      );

      _setState(ProvisioningState.keyExchanged);
      return true;
    } catch (e) {
      _setState(ProvisioningState.failed);
      rethrow;
    }
  }

  // ========================================================================
  // WiFi Scanning
  // ========================================================================

  /// Trigger WiFi scan on ESP32 and get results
  Future<List<WiFiNetwork>> scanNetworks() async {
    try {
      _setState(ProvisioningState.scanning);

      // 1. Trigger scan
      await _bleDataSource.writeCharacteristic(
        BLEUUIDs.wifiScanService,
        BLEUUIDs.scanTrigger,
        '1',
      );

      // 2. Wait for scan to complete (ESP32 takes ~2-3 seconds)
      await Future.delayed(const Duration(seconds: 3));

      // 3. Read scan results (JSON array)
      final resultsJson = await _bleDataSource.readCharacteristic(
        BLEUUIDs.wifiScanService,
        BLEUUIDs.scanResults,
      );

      // 4. Parse results
      final networks = WiFiNetwork.listFromJson(resultsJson);

      _setState(ProvisioningState.scanned);
      return networks;
    } catch (e) {
      _setState(ProvisioningState.failed);
      rethrow;
    }
  }

  // ========================================================================
  // Credential Provisioning
  // ========================================================================

  /// Send WiFi credentials to ESP32 and monitor connection
  /// Returns true if ESP32 successfully connects to WiFi
  Future<bool> provisionDevice(
    String ssid,
    String password, {
    Duration timeout = const Duration(seconds: 45),
  }) async {
    try {
      _setState(ProvisioningState.sendingCredentials);

      // 1. Subscribe to status notifications BEFORE sending credentials
      final statusCompleter = Completer<bool>();
      _statusSubscription = _bleDataSource
          .subscribeToCharacteristic(
            BLEUUIDs.credentialService,
            BLEUUIDs.credentialStatus,
          )
          .listen((status) {
            if (status == 'SUCCESS') {
              _setState(ProvisioningState.success);
              if (!statusCompleter.isCompleted) {
                statusCompleter.complete(true);
              }
            } else if (status == 'FAILED') {
              _setState(ProvisioningState.failed);
              if (!statusCompleter.isCompleted) {
                statusCompleter.complete(false);
              }
            }
          });

      // 2. Send SSID (plain text)
      await _bleDataSource.writeCharacteristic(
        BLEUUIDs.credentialService,
        BLEUUIDs.credentialSSID,
        ssid,
      );

      // 3. Encrypt and send password
      final encryptedPassword = _cryptoService.encryptPassword(password);
      await _bleDataSource.writeCharacteristic(
        BLEUUIDs.credentialService,
        BLEUUIDs.credentialPass,
        encryptedPassword,
      );

      // 4. Trigger connection attempt
      _setState(ProvisioningState.connecting);
      await _bleDataSource.writeCharacteristic(
        BLEUUIDs.credentialService,
        BLEUUIDs.credentialConnect,
        '1',
      );

      // 5. Wait for status notification or timeout
      final success = await statusCompleter.future.timeout(
        timeout,
        onTimeout: () {
          _setState(ProvisioningState.failed);
          return false;
        },
      );

      return success;
    } catch (e) {
      _setState(ProvisioningState.failed);
      rethrow;
    } finally {
      await _statusSubscription?.cancel();
      _statusSubscription = null;
    }
  }

  // ========================================================================
  // Complete Provisioning Flow
  // ========================================================================

  /// Execute complete provisioning flow:
  /// Connect → Key Exchange → WiFi Scan → Provision
  Future<ProvisioningResult> completeProvisioning({
    required PixelTreeDevice device,
    required String ssid,
    required String password,
  }) async {
    try {
      // Step 1: Connect
      final connected = await connectToDevice(device);
      if (!connected) {
        return ProvisioningResult.connectionFailed;
      }

      // Step 2: Key Exchange
      final keyExchanged = await performKeyExchange();
      if (!keyExchanged) {
        return ProvisioningResult.keyExchangeFailed;
      }

      // Step 3: Provision (includes WiFi connection attempt)
      final provisioned = await provisionDevice(ssid, password);
      if (!provisioned) {
        return ProvisioningResult.wifiConnectionFailed;
      }

      return ProvisioningResult.success;
    } catch (e) {
      return ProvisioningResult.unknownError;
    }
  }

  // ========================================================================
  // Cleanup
  // ========================================================================

  Future<void> dispose() async {
    await disconnect();
    _cryptoService.reset();
  }
}

// ============================================================================
// Enums
// ============================================================================

enum ProvisioningState {
  idle,
  connecting,
  connected,
  keyExchange,
  keyExchanged,
  scanning,
  scanned,
  sendingCredentials,
  success,
  failed,
}

enum ProvisioningResult {
  success,
  connectionFailed,
  keyExchangeFailed,
  wifiConnectionFailed,
  unknownError,
}
