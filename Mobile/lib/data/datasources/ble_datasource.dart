import 'dart:async';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import '../../core/constants/ble_uuids.dart';
import '../models/pixeltree_device.dart';

// ============================================================================
// BLE Data Source - Low-level BLE Operations
// ============================================================================
// Handles BLE scanning, connection, and GATT operations

class BLEDataSource {
  BluetoothDevice? _connectedDevice;
  final Map<String, BluetoothCharacteristic> _characteristics = {};

  // ========================================================================
  // Device Scanning
  // ========================================================================

  /// Scan for PixelTree devices
  /// Returns list of discovered devices (filtered by service UUID)
  Future<List<PixelTreeDevice>> scanForDevices({
    Duration timeout = const Duration(seconds: 10),
  }) async {
    final List<PixelTreeDevice> devices = [];
    final Set<String> seenDeviceIds = {};

    // Check if Bluetooth is available
    if (await FlutterBluePlus.isSupported == false) {
      throw Exception('Bluetooth not supported on this device');
    }

    // Check if Bluetooth is on
    final adapterState = await FlutterBluePlus.adapterState.first;
    if (adapterState != BluetoothAdapterState.on) {
      throw Exception('Please enable Bluetooth');
    }

    // Start scanning with service UUID filter
    await FlutterBluePlus.startScan(
      withServices: [Guid(BLEUUIDs.keyExchangeService)],
      timeout: timeout,
    );

    // Listen to scan results
    final subscription = FlutterBluePlus.scanResults.listen((results) {
      for (final result in results) {
        final deviceId = result.device.remoteId.toString();

        // Skip if already seen
        if (seenDeviceIds.contains(deviceId)) continue;

        // Check if device name starts with "PixelTree"
        if (result.device.platformName.startsWith('PixelTree')) {
          devices.add(PixelTreeDevice.fromScanResult(result));
          seenDeviceIds.add(deviceId);
        }
      }
    });

    // Wait for scan timeout
    await Future.delayed(timeout);

    // Stop scanning
    await FlutterBluePlus.stopScan();
    await subscription.cancel();

    return devices;
  }

  /// Get already scanned devices (from last scan)
  Stream<List<PixelTreeDevice>> get scanResultsStream {
    return FlutterBluePlus.scanResults.map((results) {
      return results
          .where((result) => result.device.platformName.startsWith('PixelTree'))
          .map((result) => PixelTreeDevice.fromScanResult(result))
          .toList();
    });
  }

  /// Check if currently scanning
  Stream<bool> get isScanningStream => FlutterBluePlus.isScanning;

  // ========================================================================
  // Connection Management
  // ========================================================================

  /// Connect to a PixelTree device
  Future<void> connectToDevice(
    PixelTreeDevice device, {
    Duration timeout = const Duration(seconds: 30),
  }) async {
    try {
      // Disconnect from any existing device
      if (_connectedDevice != null) {
        await disconnect();
      }

      _connectedDevice = device.bluetoothDevice;

      // Connect with timeout
      await _connectedDevice!.connect(timeout: timeout, autoConnect: false);

      // Discover services and cache characteristics
      await _discoverServices();
    } catch (e) {
      _connectedDevice = null;
      rethrow;
    }
  }

  /// Disconnect from current device
  Future<void> disconnect() async {
    if (_connectedDevice != null) {
      await _connectedDevice!.disconnect();
      _connectedDevice = null;
      _characteristics.clear();
    }
  }

  /// Check if connected to a device
  bool get isConnected => _connectedDevice != null;

  /// Get connected device
  BluetoothDevice? get connectedDevice => _connectedDevice;

  /// Connection state stream
  Stream<BluetoothConnectionState> get connectionStateStream {
    if (_connectedDevice == null) {
      return Stream.value(BluetoothConnectionState.disconnected);
    }
    return _connectedDevice!.connectionState;
  }

  // ========================================================================
  // Service Discovery
  // ========================================================================

  /// Discover services and cache characteristics
  Future<void> _discoverServices() async {
    if (_connectedDevice == null) {
      throw Exception('No device connected');
    }

    final services = await _connectedDevice!.discoverServices();

    // Cache all characteristics we need
    for (final service in services) {
      final serviceUuid = service.uuid.toString();

      for (final characteristic in service.characteristics) {
        final charUuid = characteristic.uuid.toString();
        final key = '$serviceUuid:$charUuid';
        _characteristics[key] = characteristic;
      }
    }
  }

  /// Get cached characteristic
  BluetoothCharacteristic? _getCharacteristic(
    String serviceUuid,
    String characteristicUuid,
  ) {
    final key = '$serviceUuid:$characteristicUuid';
    return _characteristics[key];
  }

  // ========================================================================
  // GATT Operations
  // ========================================================================

  /// Read a characteristic value
  Future<String> readCharacteristic(
    String serviceUuid,
    String characteristicUuid,
  ) async {
    final characteristic = _getCharacteristic(serviceUuid, characteristicUuid);

    if (characteristic == null) {
      final error =
          'Characteristic not found: $characteristicUuid in service $serviceUuid';
      _characteristics.forEach((key, value) {});
      throw Exception(error);
    }

    try {
      final value = await characteristic.read();
      return String.fromCharCodes(value);
    } catch (e) {
      rethrow;
    }
  }

  /// Write to a characteristic
  Future<void> writeCharacteristic(
    String serviceUuid,
    String characteristicUuid,
    String value,
  ) async {
    final characteristic = _getCharacteristic(serviceUuid, characteristicUuid);

    if (characteristic == null) {
      final error =
          'Characteristic not found: $characteristicUuid in service $serviceUuid';
      throw Exception(error);
    }

    try {
      await characteristic.write(value.codeUnits, withoutResponse: false);
    } catch (e) {
      rethrow;
    }
  }

  /// Subscribe to characteristic notifications
  Stream<String> subscribeToCharacteristic(
    String serviceUuid,
    String characteristicUuid,
  ) {
    final characteristic = _getCharacteristic(serviceUuid, characteristicUuid);

    if (characteristic == null) {
      throw Exception('Characteristic not found: $characteristicUuid');
    }

    // Enable notifications
    characteristic.setNotifyValue(true);

    // Return stream of values
    return characteristic.lastValueStream.map((value) {
      return String.fromCharCodes(value);
    });
  }

  /// Unsubscribe from characteristic notifications
  Future<void> unsubscribeFromCharacteristic(
    String serviceUuid,
    String characteristicUuid,
  ) async {
    final characteristic = _getCharacteristic(serviceUuid, characteristicUuid);

    if (characteristic != null) {
      await characteristic.setNotifyValue(false);
    }
  }

  // ========================================================================
  // Cleanup
  // ========================================================================

  /// Dispose and cleanup resources
  Future<void> dispose() async {
    await disconnect();
    _characteristics.clear();
  }
}
