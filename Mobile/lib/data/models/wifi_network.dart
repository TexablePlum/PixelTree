import 'dart:convert';

// ============================================================================
// WiFi Network Model
// ============================================================================
// Represents a WiFi network discovered during scanning

class WiFiNetwork {
  final String ssid;
  final int rssi; // Signal strength in dBm
  final bool secure; // true if password-protected

  const WiFiNetwork({
    required this.ssid,
    required this.rssi,
    required this.secure,
  });

  // Parse from JSON (from ESP32 scan results)
  factory WiFiNetwork.fromJson(Map<String, dynamic> json) {
    return WiFiNetwork(
      ssid: json['ssid'] as String,
      rssi: json['rssi'] as int,
      secure: json['secure'] as bool,
    );
  }

  // Convert to JSON
  Map<String, dynamic> toJson() {
    return {'ssid': ssid, 'rssi': rssi, 'secure': secure};
  }

  // Parse array of networks from JSON string
  static List<WiFiNetwork> listFromJson(String jsonString) {
    try {
      final List<dynamic> jsonList = json.decode(jsonString);
      return jsonList
          .map((json) => WiFiNetwork.fromJson(json as Map<String, dynamic>))
          .toList();
    } catch (e) {
      return [];
    }
  }

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

  @override
  String toString() => 'WiFiNetwork(ssid: $ssid, rssi: $rssi, secure: $secure)';

  @override
  bool operator ==(Object other) {
    if (identical(this, other)) return true;
    return other is WiFiNetwork && other.ssid == ssid;
  }

  @override
  int get hashCode => ssid.hashCode;
}

// Signal strength categories
enum SignalStrength {
  excellent, // >= -50 dBm
  good, // >= -60 dBm
  fair, // >= -70 dBm
  poor, // < -70 dBm
}
