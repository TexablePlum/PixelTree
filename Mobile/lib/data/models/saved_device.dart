// ============================================================================
// SavedDevice Model
// ============================================================================
// Represents a saved PixelTree device (only BLE+WiFi provisioned devices)

class SavedDevice {
  final String deviceId; // MAC suffix: "A1B2"
  final String name; // "PixelTree-A1B2"
  final DateTime lastSeen; // Last successful connection
  final String? ipAddress; // Last known IP address
  final String? wifiSSID; // WiFi network name device is connected to

  const SavedDevice({
    required this.deviceId,
    required this.name,
    required this.lastSeen,
    this.ipAddress,
    this.wifiSSID,
  });

  // Serialization for SharedPreferences
  Map<String, dynamic> toJson() => {
    'deviceId': deviceId,
    'name': name,
    'lastSeen': lastSeen.toIso8601String(),
    'ipAddress': ipAddress,
    'wifiSSID': wifiSSID,
  };

  factory SavedDevice.fromJson(Map<String, dynamic> json) => SavedDevice(
    deviceId: json['deviceId'] as String,
    name: json['name'] as String,
    lastSeen: DateTime.parse(json['lastSeen'] as String),
    ipAddress: json['ipAddress'] as String?,
    wifiSSID: json['wifiSSID'] as String?,
  );

  // Copy with updated fields
  SavedDevice copyWith({
    DateTime? lastSeen,
    String? ipAddress,
    String? wifiSSID,
  }) => SavedDevice(
    deviceId: deviceId,
    name: name,
    lastSeen: lastSeen ?? this.lastSeen,
    ipAddress: ipAddress ?? this.ipAddress,
    wifiSSID: wifiSSID ?? this.wifiSSID,
  );

  @override
  String toString() =>
      'SavedDevice(id: $deviceId, name: $name, ip: $ipAddress, ssid: $wifiSSID)';

  @override
  bool operator ==(Object other) {
    if (identical(this, other)) return true;
    return other is SavedDevice && other.deviceId == deviceId;
  }

  @override
  int get hashCode => deviceId.hashCode;
}
