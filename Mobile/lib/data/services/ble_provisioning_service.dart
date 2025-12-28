import 'dart:async';
import '../../core/constants/ble_uuids.dart';
import '../datasources/ble_datasource.dart';
import '../datasources/crypto_datasource.dart';
import '../models/wifi_network.dart';
import '../models/pixeltree_device.dart';
import 'provisioning_service.dart';

// ============================================================================
// BLE Provisioning Service
// ============================================================================
// BLE + WiFi provisioning implementation using ECDH encryption

class BLEProvisioningService implements ProvisioningService {
  final BLEDataSource _bleDataSource;
  final ECDHCryptoService _cryptoService;
  final PixelTreeDevice device;

  ProvisioningState _state = ProvisioningState.idle;
  final _stateController = StreamController<ProvisioningState>.broadcast();
  StreamSubscription<String>? _statusSubscription;

  BLEProvisioningService({
    required this.device,
    BLEDataSource? bleDataSource,
    ECDHCryptoService? cryptoService,
  }) : _bleDataSource = bleDataSource ?? BLEDataSource(),
       _cryptoService = cryptoService ?? ECDHCryptoService();

  @override
  ProvisioningState get state => _state;

  @override
  Stream<ProvisioningState> get stateStream => _stateController.stream;

  void _setState(ProvisioningState newState) {
    _state = newState;
    _stateController.add(newState);
  }

  // ========================================================================
  // Initialize (Connect to BLE device)
  // ========================================================================

  @override
  Future<void> initialize() async {
    try {
      _setState(ProvisioningState.initializing);
      await _bleDataSource.connectToDevice(device);
      _setState(ProvisioningState.connected);
    } catch (e) {
      _setState(ProvisioningState.failed);
      rethrow;
    }
  }

  // ========================================================================
  // Key Exchange (ECDH)
  // ========================================================================

  @override
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

  @override
  Future<List<WiFiNetwork>> scanNetworks() async {
    try {
      _setState(ProvisioningState.scanning);

      // 1. Trigger scan
      await _bleDataSource.writeCharacteristic(
        BLEUUIDs.wifiScanService,
        BLEUUIDs.scanTrigger,
        '1',
      );

      // 2. Wait for scan to complete - ESP32 needs time to scan networks
      // First wait: give ESP time to start and complete scan
      await Future.delayed(const Duration(seconds: 6));

      // 3. Read scan results with retry
      String resultsJson = '';
      int retries = 0;
      const maxRetries = 3;

      while (retries < maxRetries) {
        try {
          resultsJson = await _bleDataSource.readCharacteristic(
            BLEUUIDs.wifiScanService,
            BLEUUIDs.scanResults,
          );

          // Check if we got valid results (not empty array)
          if (resultsJson.isNotEmpty && resultsJson != '[]') {
            break;
          }

          await Future.delayed(const Duration(seconds: 2));
          retries++;
        } catch (e) {
          retries++;
          if (retries >= maxRetries) rethrow;
          await Future.delayed(const Duration(seconds: 2));
        }
      }

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
  // Provisioning
  // ========================================================================

  @override
  Future<bool> provisionDevice(
    String ssid,
    String password, {
    Duration timeout = const Duration(seconds: 45),
  }) async {
    // Cancel any existing subscription from previous attempt
    if (_statusSubscription != null) {
      await _statusSubscription?.cancel();
      _statusSubscription = null;
    }

    _setState(ProvisioningState.sendingCredentials);

    // Track the received status - only process AFTER trigger is sent
    final statusCompleter = Completer<String>();
    bool readyForStatus =
        false; // Flag to ignore cached status from previous attempt

    try {
      // 1. Subscribe to status notifications
      _statusSubscription = _bleDataSource
          .subscribeToCharacteristic(
            BLEUUIDs.credentialService,
            BLEUUIDs.credentialStatus,
          )
          .listen((status) {
            // Ignore empty status
            if (status.isEmpty) {
              return;
            }

            // Ignore status that comes BEFORE we sent the trigger
            // This filters out cached status from previous attempt
            if (!readyForStatus) {
              return;
            }

            // Complete with the received status
            if (!statusCompleter.isCompleted) {
              statusCompleter.complete(status);
            }
          });

      // 2. Send SSID
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

      // 4. Trigger connection - NOW we're ready to receive status!
      _setState(ProvisioningState.connecting);

      // Enable status processing BEFORE sending trigger
      readyForStatus = true;

      await _bleDataSource.writeCharacteristic(
        BLEUUIDs.credentialService,
        BLEUUIDs.credentialConnect,
        '1',
      );

      // 5. Wait for result
      final status = await statusCompleter.future.timeout(
        timeout,
        onTimeout: () => 'TIMEOUT',
      );

      // 6. Handle the status and throw appropriate exception
      switch (status) {
        case 'SUCCESS':
          _setState(ProvisioningState.success);
          return true;

        case 'WRONG_PASSWORD':
          _setState(ProvisioningState.wrongPassword);
          throw WrongPasswordException();

        case 'SSID_NOT_FOUND':
          _setState(ProvisioningState.ssidNotFound);
          throw SsidNotFoundException();

        case 'AUTH_FAILED':
          _setState(ProvisioningState.authFailed);
          throw AuthFailedException();

        case 'TIMEOUT':
        case 'FAILED':
        default:
          _setState(ProvisioningState.failed);
          throw ConnectionTimeoutException();
      }
    } finally {
      await _statusSubscription?.cancel();
      _statusSubscription = null;
    }
  }

  // ========================================================================
  // Cleanup
  // ========================================================================

  @override
  Future<void> dispose() async {
    await _statusSubscription?.cancel();
    await _bleDataSource.dispose();
    _cryptoService.reset();
    await _stateController.close();
  }
}
