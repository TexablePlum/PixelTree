import '../datasources/device_storage.dart';
import '../models/saved_device.dart';

// ============================================================================
// DeviceRepository - Business Logic Layer
// ============================================================================

class DeviceRepository {
  final DeviceStorage _storage;

  DeviceRepository(this._storage);

  // Get all devices
  Future<List<SavedDevice>> getDevices() => _storage.loadDevices();

  // Save device
  Future<void> saveDevice(SavedDevice device) => _storage.saveDevice(device);

  // Remove device
  Future<void> removeDevice(String deviceId) => _storage.removeDevice(deviceId);

  // Check if has any devices
  Future<bool> hasDevices() async {
    final devices = await getDevices();
    return devices.isNotEmpty;
  }

  // Clear all devices
  Future<void> clearAll() => _storage.clearAll();
}
