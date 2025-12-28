import '../models/wifi_network.dart';

// ============================================================================
// Provisioning Service - Abstract Interface
// ============================================================================
// Common interface for both BLE and HTTP provisioning methods

abstract class ProvisioningService {
  /// Initialize the provisioning service
  /// For BLE: connect to device
  /// For HTTP: verify AP connection
  Future<void> initialize();

  /// Perform key exchange (BLE only)
  /// For HTTP: returns true immediately (no encryption needed)
  Future<bool> performKeyExchange() async => true;

  /// Scan for available WiFi networks
  /// Returns list of discovered networks
  Future<List<WiFiNetwork>> scanNetworks();

  /// Provision device with WiFi credentials
  /// Returns true if successful, false otherwise
  Future<bool> provisionDevice(String ssid, String password);

  /// Get current provisioning state
  ProvisioningState get state;

  /// Stream of provisioning state changes
  Stream<ProvisioningState> get stateStream;

  /// Cleanup resources
  Future<void> dispose();
}

// ==============================================================================
// Provisioning State Enum
// ============================================================================

enum ProvisioningState {
  idle,
  initializing,
  connected,
  keyExchange,
  keyExchanged,
  scanning,
  scanned,
  sendingCredentials,
  connecting,
  success,
  failed,
  wrongPassword, // Wrong WiFi password
  ssidNotFound, // Network not found / disappeared
  authFailed, // Other authentication failure
}

// ============================================================================
// Provisioning Result Enum
// ============================================================================

enum ProvisioningResult {
  success,
  connectionFailed,
  keyExchangeFailed,
  wifiConnectionFailed,
  unknownError,
}

// ============================================================================
// Provisioning Exceptions
// ============================================================================

/// Thrown when WiFi password is incorrect
class WrongPasswordException implements Exception {
  @override
  String toString() => 'Wrong WiFi password';
}

/// Thrown when WiFi network is not found
class SsidNotFoundException implements Exception {
  @override
  String toString() => 'WiFi network not found';
}

/// Thrown when WiFi authentication fails
class AuthFailedException implements Exception {
  @override
  String toString() => 'WiFi authentication failed';
}

/// Thrown when connection times out
class ConnectionTimeoutException implements Exception {
  @override
  String toString() => 'Connection timeout';
}
