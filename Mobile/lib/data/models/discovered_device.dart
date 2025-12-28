// ============================================================================
// DiscoveredDevice Model
// ============================================================================
// Represents a PixelTree device found on network via mDNS (not yet saved)

class DiscoveredDevice {
  final String deviceId; // MAC suffix: "A1B2"
  final String name; // "PixelTree-A1B2"
  final String host; // IP or hostname for connection
  final String? ipAddress; // IP address if resolved
  final String? ssid; // WiFi network name

  const DiscoveredDevice({
    required this.deviceId,
    required this.name,
    required this.host,
    this.ipAddress,
    this.ssid,
  });

  @override
  String toString() =>
      'DiscoveredDevice(id: $deviceId, name: $name, host: $host)';

  @override
  bool operator ==(Object other) {
    if (identical(this, other)) return true;
    return other is DiscoveredDevice && other.deviceId == deviceId;
  }

  @override
  int get hashCode => deviceId.hashCode;
}
