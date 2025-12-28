// ============================================================================
// BLE UUIDs - PixelTree Service and Characteristics
// ============================================================================
// These UUIDs must match exactly with ESP32 firmware

class BLEUUIDs {
  // Key Exchange Service - ECDH P-256
  static const String keyExchangeService =
      '8e400001-b5a3-f393-e0a9-e50e24dcca9e';
  static const String publicKeyESP = '8e400002-b5a3-f393-e0a9-e50e24dcca9e';
  static const String publicKeyApp = '8e400003-b5a3-f393-e0a9-e50e24dcca9e';

  // WiFi Scan Service
  static const String wifiScanService = '6e400001-b5a3-f393-e0a9-e50e24dcca9e';
  static const String scanTrigger = '6e400002-b5a3-f393-e0a9-e50e24dcca9e';
  static const String scanResults = '6e400003-b5a3-f393-e0a9-e50e24dcca9e';

  // Credential Service
  static const String credentialService =
      '7e400001-b5a3-f393-e0a9-e50e24dcca9e';
  static const String credentialSSID = '7e400002-b5a3-f393-e0a9-e50e24dcca9e';
  static const String credentialPass = '7e400003-b5a3-f393-e0a9-e50e24dcca9e';
  static const String credentialConnect =
      '7e400004-b5a3-f393-e0a9-e50e24dcca9e';
  static const String credentialStatus = '7e400005-b5a3-f393-e0a9-e50e24dcca9e';
}
