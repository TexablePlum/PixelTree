import 'dart:convert';
import 'package:shared_preferences/shared_preferences.dart';
import '../models/saved_device.dart';

// ============================================================================
// DeviceStorage - Local Persistence
// ============================================================================
// Stores saved devices using SharedPreferences

class DeviceStorage {
  static const String _storageKey = 'saved_devices';
  final SharedPreferences _prefs;

  DeviceStorage(this._prefs);

  // Load all saved devices
  Future<List<SavedDevice>> loadDevices() async {
    final jsonString = _prefs.getString(_storageKey);
    if (jsonString == null) return [];

    try {
      final List<dynamic> jsonList = json.decode(jsonString);
      return jsonList.map((j) => SavedDevice.fromJson(j)).toList();
    } catch (e) {
      // Corrupted data - return empty
      return [];
    }
  }

  // Save device (update if exists, add if new)
  Future<void> saveDevice(SavedDevice device) async {
    final devices = await loadDevices();

    // Remove if already exists
    devices.removeWhere((d) => d.deviceId == device.deviceId);

    // Add at beginning (most recent first)
    devices.insert(0, device);

    await _saveAll(devices);
  }

  // Remove device by ID
  Future<void> removeDevice(String deviceId) async {
    final devices = await loadDevices();
    devices.removeWhere((d) => d.deviceId == deviceId);
    await _saveAll(devices);
  }

  // Clear all devices
  Future<void> clearAll() async {
    await _prefs.remove(_storageKey);
  }

  // Private: persist to storage
  Future<void> _saveAll(List<SavedDevice> devices) async {
    final jsonList = devices.map((d) => d.toJson()).toList();
    await _prefs.setString(_storageKey, json.encode(jsonList));
  }
}
