import 'package:flutter/material.dart';
import 'package:permission_handler/permission_handler.dart';
import '../../../core/constants/app_colors.dart';
import '../../../l10n/app_localizations.dart';
import '../../../data/datasources/ble_datasource.dart';
import '../../../data/models/pixeltree_device.dart';
import '../../../data/services/ble_provisioning_service.dart';
import '../provisioning/provisioning_wizard_screen.dart';

// ============================================================================
// Device Scan Screen - Find PixelTree Devices via BLE
// ============================================================================

class DeviceScanScreen extends StatefulWidget {
  const DeviceScanScreen({super.key});

  @override
  State<DeviceScanScreen> createState() => _DeviceScanScreenState();
}

class _DeviceScanScreenState extends State<DeviceScanScreen> {
  final BLEDataSource _bleDataSource = BLEDataSource();

  bool _isScanning = false;
  List<PixelTreeDevice> _devices = [];
  String? _errorMessage;
  bool _permissionsChecked = false;

  @override
  void initState() {
    super.initState();
  }

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    if (!_permissionsChecked) {
      _permissionsChecked = true;
      _checkPermissions();
    }
  }

  @override
  void dispose() {
    _bleDataSource.dispose();
    super.dispose();
  }

  // ========================================================================
  // Permissions
  // ========================================================================

  Future<void> _checkPermissions() async {
    // Request Bluetooth permissions
    final bluetoothStatus = await Permission.bluetooth.request();
    final bluetoothScanStatus = await Permission.bluetoothScan.request();
    final bluetoothConnectStatus = await Permission.bluetoothConnect.request();
    final locationStatus = await Permission.locationWhenInUse.request();

    if (bluetoothStatus.isGranted &&
        bluetoothScanStatus.isGranted &&
        bluetoothConnectStatus.isGranted &&
        locationStatus.isGranted) {
      // Permissions granted - start scanning
      _startScan();
    } else {
      if (mounted) {
        setState(() {
          _errorMessage =
              AppLocalizations.of(context)?.errorBluetoothPermission ??
              'Bluetooth permission required';
        });
      }
    }
  }

  // ========================================================================
  // BLE Scanning
  // ========================================================================

  Future<void> _startScan() async {
    if (_isScanning) return;

    setState(() {
      _isScanning = true;
      _errorMessage = null;
      _devices.clear();
    });

    try {
      final devices = await _bleDataSource.scanForDevices(
        timeout: const Duration(seconds: 10),
      );

      if (mounted) {
        setState(() {
          _devices = devices;
          _isScanning = false;
        });
      }
    } catch (e) {
      if (mounted) {
        setState(() {
          _errorMessage = e.toString();
          _isScanning = false;
        });
      }
    }
  }

  // ========================================================================
  // UI
  // ========================================================================

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(
          AppLocalizations.of(context)?.scanTitle ?? 'Find Your PixelTree',
        ),
      ),
      body: Column(
        children: [
          // Scanning indicator
          if (_isScanning)
            Container(
              padding: const EdgeInsets.all(32),
              child: Column(
                children: [
                  const CircularProgressIndicator(),
                  const SizedBox(height: 16),
                  Text(
                    AppLocalizations.of(context)?.scanScanning ??
                        'Searching for devices...',
                    style: Theme.of(context).textTheme.bodyLarge,
                  ),
                ],
              ),
            ),

          // Error message
          if (_errorMessage != null && !_isScanning)
            Container(
              margin: const EdgeInsets.all(16),
              padding: const EdgeInsets.all(16),
              decoration: BoxDecoration(
                color: AppColors.error.withValues(alpha: 0.1),
                borderRadius: BorderRadius.circular(12),
                border: Border.all(color: AppColors.error),
              ),
              child: Row(
                children: [
                  const Icon(Icons.error, color: AppColors.error),
                  const SizedBox(width: 12),
                  Expanded(
                    child: Text(
                      _errorMessage!,
                      style: const TextStyle(color: AppColors.error),
                    ),
                  ),
                ],
              ),
            ),

          // Devices list
          Expanded(child: _buildDevicesList()),

          // Refresh button
          Padding(
            padding: const EdgeInsets.all(16.0),
            child: ElevatedButton.icon(
              onPressed: _isScanning ? null : _startScan,
              icon: const Icon(Icons.refresh),
              label: Text(
                AppLocalizations.of(context)?.scanRefresh ?? 'Refresh Scan',
              ),
              style: ElevatedButton.styleFrom(
                minimumSize: const Size(double.infinity, 56),
              ),
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildDevicesList() {
    if (_devices.isEmpty && !_isScanning) {
      return Center(
        child: Padding(
          padding: const EdgeInsets.all(32.0),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Icon(
                Icons.bluetooth_searching,
                size: 64,
                color: Colors.grey[400],
              ),
              const SizedBox(height: 16),
              Text(
                AppLocalizations.of(context)?.scanNoDevices ??
                    'No devices found.',
                style: Theme.of(context).textTheme.bodyLarge,
                textAlign: TextAlign.center,
              ),
            ],
          ),
        ),
      );
    }

    return ListView.builder(
      padding: const EdgeInsets.all(16),
      itemCount: _devices.length,
      itemBuilder: (context, index) {
        final device = _devices[index];
        return _DeviceCard(
          device: device,
          onConnect: () => _connectToDevice(device),
        );
      },
    );
  }

  Future<void> _connectToDevice(PixelTreeDevice device) async {
    // Navigate to provisioning wizard with BLE service
    Navigator.of(context).push(
      MaterialPageRoute(
        builder: (_) => ProvisioningWizardScreen(
          service: BLEProvisioningService(device: device),
        ),
      ),
    );
  }
}

// ============================================================================
// Device Card Widget
// ============================================================================

class _DeviceCard extends StatelessWidget {
  final PixelTreeDevice device;
  final VoidCallback onConnect;

  const _DeviceCard({required this.device, required this.onConnect});

  @override
  Widget build(BuildContext context) {
    // Signal strength bars
    final signalStrength = device.getSignalBars();

    return Card(
      margin: const EdgeInsets.only(bottom: 12),
      child: InkWell(
        onTap: onConnect,
        borderRadius: BorderRadius.circular(16),
        child: Padding(
          padding: const EdgeInsets.all(16.0),
          child: Row(
            children: [
              // Device icon
              Container(
                width: 56,
                height: 56,
                decoration: BoxDecoration(
                  color: AppColors.forestGreen.withValues(alpha: 0.1),
                  borderRadius: BorderRadius.circular(12),
                ),
                child: const Icon(
                  Icons.park,
                  color: AppColors.forestGreen,
                  size: 32,
                ),
              ),

              const SizedBox(width: 16),

              // Device info
              Expanded(
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      device.name,
                      style: Theme.of(context).textTheme.titleMedium?.copyWith(
                        fontWeight: FontWeight.w600,
                      ),
                    ),
                    const SizedBox(height: 4),
                    Row(
                      children: [
                        // Signal bars
                        ...List.generate(
                          4,
                          (index) => Container(
                            width: 4,
                            height: 12 - (index * 2.0),
                            margin: const EdgeInsets.only(right: 2),
                            decoration: BoxDecoration(
                              color: index < signalStrength
                                  ? AppColors.success
                                  : Colors.grey[300],
                              borderRadius: BorderRadius.circular(2),
                            ),
                          ),
                        ),
                        const SizedBox(width: 8),
                        Text(
                          '${device.rssi} dBm',
                          style: Theme.of(context).textTheme.bodySmall,
                        ),
                      ],
                    ),
                    const SizedBox(height: 4),
                    Text(
                      device.isConfigured
                          ? (AppLocalizations.of(context)?.scanConfigured ??
                                'Configured')
                          : (AppLocalizations.of(context)?.scanReadyToConnect ??
                                'Ready to connect'),
                      style: Theme.of(context).textTheme.bodySmall?.copyWith(
                        color: device.isConfigured
                            ? AppColors.success
                            : AppColors.warning,
                      ),
                    ),
                  ],
                ),
              ),

              // Connect icon
              const Icon(Icons.chevron_right, color: AppColors.forestGreen),
            ],
          ),
        ),
      ),
    );
  }
}
