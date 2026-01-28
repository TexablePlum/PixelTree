import 'package:flutter_test/flutter_test.dart';
import 'package:mobile/data/models/saved_device.dart';
import 'package:mobile/data/models/wifi_network.dart';
import 'package:mobile/data/models/effect.dart';

void main() {
  // ==========================================================================
  // SavedDevice Tests
  // ==========================================================================
  group('SavedDevice', () {
    test('fromJson correctly parses JSON', () {
      final json = {
        'deviceId': 'A1B2',
        'name': 'PixelTree-A1B2',
        'lastSeen': '2024-01-15T12:30:00.000',
        'ipAddress': '192.168.1.100',
        'wifiSSID': 'HomeWiFi',
      };

      final device = SavedDevice.fromJson(json);

      expect(device.deviceId, 'A1B2');
      expect(device.name, 'PixelTree-A1B2');
      expect(device.ipAddress, '192.168.1.100');
      expect(device.wifiSSID, 'HomeWiFi');
    });

    test('toJson creates correct JSON', () {
      final device = SavedDevice(
        deviceId: 'C3D4',
        name: 'PixelTree-C3D4',
        lastSeen: DateTime(2024, 1, 15, 12, 30),
        ipAddress: '192.168.1.50',
        wifiSSID: 'TestNetwork',
      );

      final json = device.toJson();

      expect(json['deviceId'], 'C3D4');
      expect(json['name'], 'PixelTree-C3D4');
      expect(json['ipAddress'], '192.168.1.50');
    });

    test('fromJson handles null optional fields', () {
      final json = {
        'deviceId': 'E5F6',
        'name': 'PixelTree-E5F6',
        'lastSeen': '2024-01-15T12:30:00.000',
        'ipAddress': null,
        'wifiSSID': null,
      };

      final device = SavedDevice.fromJson(json);

      expect(device.ipAddress, isNull);
      expect(device.wifiSSID, isNull);
    });
  });

  // ==========================================================================
  // WiFiNetwork Tests
  // ==========================================================================
  group('WiFiNetwork', () {
    test('fromJson correctly parses JSON', () {
      final json = {'ssid': 'TestNetwork', 'rssi': -55, 'secure': true};

      final network = WiFiNetwork.fromJson(json);

      expect(network.ssid, 'TestNetwork');
      expect(network.rssi, -55);
      expect(network.secure, true);
    });

    test('getSignalStrength returns correct category', () {
      expect(
        WiFiNetwork(ssid: 'A', rssi: -45, secure: true).getSignalStrength(),
        SignalStrength.excellent,
      );
      expect(
        WiFiNetwork(ssid: 'B', rssi: -55, secure: true).getSignalStrength(),
        SignalStrength.good,
      );
      expect(
        WiFiNetwork(ssid: 'C', rssi: -65, secure: true).getSignalStrength(),
        SignalStrength.fair,
      );
      expect(
        WiFiNetwork(ssid: 'D', rssi: -80, secure: true).getSignalStrength(),
        SignalStrength.poor,
      );
    });

    test('getSignalBars returns correct number', () {
      expect(
        WiFiNetwork(ssid: 'A', rssi: -45, secure: true).getSignalBars(),
        4,
      );
      expect(
        WiFiNetwork(ssid: 'B', rssi: -55, secure: true).getSignalBars(),
        3,
      );
      expect(
        WiFiNetwork(ssid: 'C', rssi: -65, secure: true).getSignalBars(),
        2,
      );
      expect(
        WiFiNetwork(ssid: 'D', rssi: -80, secure: true).getSignalBars(),
        1,
      );
    });

    test('listFromJson parses array correctly', () {
      final jsonString = '''
        [
          {"ssid": "Network1", "rssi": -50, "secure": true},
          {"ssid": "Network2", "rssi": -70, "secure": false}
        ]
      ''';

      final networks = WiFiNetwork.listFromJson(jsonString);

      expect(networks.length, 2);
      expect(networks[0].ssid, 'Network1');
      expect(networks[1].ssid, 'Network2');
    });

    test('listFromJson returns empty list on invalid JSON', () {
      final networks = WiFiNetwork.listFromJson('invalid json');
      expect(networks, isEmpty);
    });
  });

  // ==========================================================================
  // LEDStatus Tests
  // ==========================================================================
  group('LEDStatus', () {
    test('fromJson correctly parses full response', () {
      final json = {
        'power': true,
        'brightness': 200,
        'effect': 5,
        'effectName': 'Rainbow Wave',
        'category': 2,
        'numEffects': 42,
      };

      final status = LEDStatus.fromJson(json);

      expect(status.power, true);
      expect(status.brightness, 200);
      expect(status.effectId, 5);
      expect(status.effectName, 'Rainbow Wave');
      expect(status.category, 2);
      expect(status.numEffects, 42);
    });

    test('fromJson uses default values for missing fields', () {
      final json = <String, dynamic>{};

      final status = LEDStatus.fromJson(json);

      expect(status.power, false);
      expect(status.brightness, 180);
      expect(status.effectId, 0);
      expect(status.effectName, 'Unknown');
    });

    test('fromJson handles partial response', () {
      final json = {'power': true, 'brightness': 100};

      final status = LEDStatus.fromJson(json);

      expect(status.power, true);
      expect(status.brightness, 100);
      expect(status.effectId, 0);
    });
  });
}
