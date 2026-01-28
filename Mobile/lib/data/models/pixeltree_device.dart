import 'package:flutter_blue_plus/flutter_blue_plus.dart';

// ============================================================================
// PixelTree Device Model
// ============================================================================
// Represents a discovered PixelTree device via BLE

class PixelTreeDevice {
  final BluetoothDevice bluetoothDevice;
  final String name; // e.g., "PixelTree-A1B2"
  final int rssi; // Signal strength in dBm
  final bool isConfigured; // Has WiFi credentials (optional)

  const PixelTreeDevice({
    required this.bluetoothDevice,
    required this.name,
    required this.rssi,
    this.isConfigured = false,
  });

  // Create from BLE scan result
  factory PixelTreeDevice.fromScanResult(ScanResult scanResult) {
    return PixelTreeDevice(
      bluetoothDevice: scanResult.device,
      name: scanResult.device.platformName.isNotEmpty
          ? scanResult.device.platformName
          : 'PixelTree-XXXX',
      rssi: scanResult.rssi,
      isConfigured: false, // Unknown until we connect
    );
  }

  // Extract MAC suffix from device name (e.g., "A1B2" from "PixelTree-A1B2")
  String get macSuffix {
    final parts = name.split('-');
    if (parts.length >= 2) {
      return parts.last;
    }
    return 'XXXX';
  }

  // Get device ID (remote ID)
  String get deviceId => bluetoothDevice.remoteId.toString();

  // Check if this is a valid PixelTree device
  bool get isValid => name.startsWith('PixelTree-');

  // Get signal strength category
  SignalStrength getSignalStrength() {
    if (rssi >= -50) return SignalStrength.excellent;
    if (rssi >= -60) return SignalStrength.good;
    if (rssi >= -70) return SignalStrength.fair;
    return SignalStrength.poor;
  }

  // Get number of signal bars (1-4)
  int getSignalBars() {
    if (rssi >= -50) return 4;
    if (rssi >= -60) return 3;
    if (rssi >= -70) return 2;
    return 1;
  }

  // Copy with updated fields
  PixelTreeDevice copyWith({
    BluetoothDevice? bluetoothDevice,
    String? name,
    int? rssi,
    bool? isConfigured,
  }) {
    return PixelTreeDevice(
      bluetoothDevice: bluetoothDevice ?? this.bluetoothDevice,
      name: name ?? this.name,
      rssi: rssi ?? this.rssi,
      isConfigured: isConfigured ?? this.isConfigured,
    );
  }

  @override
  String toString() =>
      'PixelTreeDevice(name: $name, rssi: $rssi, configured: $isConfigured)';

  @override
  bool operator ==(Object other) {
    if (identical(this, other)) return true;
    return other is PixelTreeDevice && other.deviceId == deviceId;
  }

  @override
  int get hashCode => deviceId.hashCode;
}

// Signal strength categories
enum SignalStrength {
  excellent, // >= -50 dBm
  good, // >= -60 dBm
  fair, // >= -70 dBm
  poor, // < -70 dBm
}
