import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';
import '../../../core/constants/app_colors.dart';
import '../../../l10n/app_localizations.dart';
import '../../../data/datasources/device_storage.dart';
import '../../../data/datasources/http_datasource.dart';
import '../../../data/models/discovered_device.dart';
import '../../../data/models/saved_device.dart';
import '../../../data/repositories/device_repository.dart';
import '../../../data/services/device_discovery_service.dart';
import '../connection_mode/connection_mode_screen.dart';
import '../device_connected/device_connected_screen.dart';

// ============================================================================
// My Devices Screen - Saved Devices List
// ============================================================================

class MyDevicesScreen extends StatefulWidget {
  const MyDevicesScreen({super.key});

  @override
  State<MyDevicesScreen> createState() => _MyDevicesScreenState();
}

class _MyDevicesScreenState extends State<MyDevicesScreen> {
  late DeviceRepository _repository;
  List<SavedDevice> _devices = [];
  bool _loading = true;

  // Track online status for each device
  final Map<String, bool> _deviceAvailability = {};
  bool _checkingAvailability = false;

  // Discovered devices from network scan
  List<DiscoveredDevice> _discoveredDevices = [];
  bool _scanningNetwork = false;

  @override
  void initState() {
    super.initState();
    _loadDevices();
  }

  Future<void> _loadDevices() async {
    final prefs = await SharedPreferences.getInstance();
    _repository = DeviceRepository(DeviceStorage(prefs));

    final devices = await _repository.getDevices();
    if (mounted) {
      setState(() {
        _devices = devices;
        _loading = false;
      });

      // Start background availability check and network scan
      _checkDevicesAvailability();
      _scanNetworkForDevices();
    }
  }

  /// Scan network for all PixelTree devices via mDNS
  Future<void> _scanNetworkForDevices() async {
    if (_scanningNetwork) return;

    setState(() {
      _scanningNetwork = true;
    });

    final httpDataSource = HttpDataSource();
    final discoveryService = DeviceDiscoveryService(httpDataSource);

    try {
      final devices = await discoveryService.discoverAllDevices();

      if (mounted) {
        // Filter out already saved devices
        final savedIds = _devices.map((d) => d.deviceId).toSet();
        final newDevices = devices
            .where((d) => !savedIds.contains(d.deviceId))
            .toList();

        setState(() {
          _discoveredDevices = newDevices;
          _scanningNetwork = false;
        });
      }
    } catch (e) {
      if (mounted) {
        setState(() {
          _scanningNetwork = false;
        });
      }
    } finally {
      httpDataSource.dispose();
    }
  }

  /// Save a discovered device to the repository
  Future<void> _saveDiscoveredDevice(DiscoveredDevice discovered) async {
    final savedDevice = SavedDevice(
      deviceId: discovered.deviceId,
      name: discovered.name,
      lastSeen: DateTime.now(),
      ipAddress: discovered.ipAddress,
      wifiSSID: discovered.ssid,
    );

    await _repository.saveDevice(savedDevice);

    if (mounted) {
      setState(() {
        _devices.add(savedDevice);
        _discoveredDevices.removeWhere(
          (d) => d.deviceId == discovered.deviceId,
        );
        _deviceAvailability[discovered.deviceId] = true;
      });

      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text(
            AppLocalizations.of(context)?.myDevicesSaved(discovered.name) ??
                '${discovered.name} saved!',
          ),
          backgroundColor: AppColors.forestGreen,
        ),
      );
    }
  }

  /// Check availability of all devices in background
  Future<void> _checkDevicesAvailability() async {
    if (_devices.isEmpty || _checkingAvailability) return;

    setState(() {
      _checkingAvailability = true;
    });

    final httpDataSource = HttpDataSource();
    final discoveryService = DeviceDiscoveryService(httpDataSource);

    // Check each device in parallel (with short timeout)
    await Future.wait(
      _devices.map((device) async {
        try {
          final result = await discoveryService
              .discoverDevice(device.deviceId)
              .timeout(const Duration(seconds: 5), onTimeout: () => null);

          if (mounted) {
            setState(() {
              _deviceAvailability[device.deviceId] = result != null;
            });

            // Update device with IP and SSID from /api/status
            if (result != null) {
              final status = result['status'] as Map<String, dynamic>?;
              final ipAddress = status?['ip'] as String?; // Real IP from device
              final wifiSSID = status?['ssid'] as String?;

              if (ipAddress != null || wifiSSID != null) {
                final needsUpdate =
                    device.ipAddress != ipAddress ||
                    device.wifiSSID != wifiSSID;

                if (needsUpdate) {
                  final updatedDevice = SavedDevice(
                    deviceId: device.deviceId,
                    name: device.name,
                    lastSeen: DateTime.now(),
                    ipAddress: ipAddress,
                    wifiSSID: wifiSSID,
                  );
                  await _repository.saveDevice(updatedDevice);
                  final index = _devices.indexWhere(
                    (d) => d.deviceId == device.deviceId,
                  );
                  if (index != -1) _devices[index] = updatedDevice;
                }
              }
            }
          }
        } catch (e) {
          if (mounted) {
            setState(() {
              _deviceAvailability[device.deviceId] = false;
            });
          }
        }
      }),
    );

    httpDataSource.dispose();

    if (mounted) {
      setState(() {
        _checkingAvailability = false;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(
          AppLocalizations.of(context)?.myDevicesTitle ?? 'My Devices',
        ),
        actions: [
          IconButton(
            icon: const Icon(Icons.add_circle_outline),
            onPressed: () {
              Navigator.push(
                context,
                MaterialPageRoute(builder: (_) => const ConnectionModeScreen()),
              ).then((_) => _loadDevices());
            },
            tooltip:
                AppLocalizations.of(context)?.myDevicesAddDevice ??
                'Add Device',
          ),
        ],
      ),
      floatingActionButton: _loading
          ? null
          : FloatingActionButton.extended(
              onPressed: (_checkingAvailability || _scanningNetwork)
                  ? null
                  : () {
                      _checkDevicesAvailability();
                      _scanNetworkForDevices();
                    },
              icon: (_checkingAvailability || _scanningNetwork)
                  ? const SizedBox(
                      width: 20,
                      height: 20,
                      child: CircularProgressIndicator(
                        strokeWidth: 2,
                        valueColor: AlwaysStoppedAnimation<Color>(Colors.white),
                      ),
                    )
                  : const Icon(Icons.refresh),
              label: Text(
                _scanningNetwork
                    ? (AppLocalizations.of(context)?.myDevicesScanningBtn ??
                          'Scanning...')
                    : _checkingAvailability
                    ? (AppLocalizations.of(context)?.myDevicesCheckingBtn ??
                          'Checking...')
                    : (AppLocalizations.of(context)?.myDevicesRefreshBtn ??
                          'Refresh'),
              ),
              backgroundColor: (_checkingAvailability || _scanningNetwork)
                  ? Colors.grey
                  : AppColors.forestGreen,
            ),
      body: _loading
          ? const Center(child: CircularProgressIndicator())
          : _buildDevicesSections(),
    );
  }

  Widget _buildDevicesSections() {
    final l10n = AppLocalizations.of(context);
    final hasDevices = _devices.isNotEmpty || _discoveredDevices.isNotEmpty;

    if (!hasDevices && !_scanningNetwork) {
      return _buildEmptyState();
    }

    return RefreshIndicator(
      onRefresh: () async {
        await Future.wait([
          _checkDevicesAvailability(),
          _scanNetworkForDevices(),
        ]);
      },
      child: ListView(
        padding: const EdgeInsets.all(16),
        children: [
          // ========== SAVED DEVICES SECTION ==========
          _buildSectionHeader(
            icon: Icons.bookmark,
            title: l10n?.myDevicesSavedDevices ?? 'Saved Devices',
            count: _devices.length,
            color: AppColors.forestGreen,
            isLoading: _checkingAvailability,
          ),
          const SizedBox(height: 12),
          if (_devices.isEmpty)
            _buildEmptySectionMessage(
              l10n?.myDevicesNoSaved ?? 'No saved devices yet',
              l10n?.myDevicesNoSavedSubtitle ??
                  'Add devices via BLE or save discovered ones below',
            )
          else
            ..._devices.map(
              (device) => Dismissible(
                key: Key(device.deviceId),
                direction: DismissDirection.endToStart,
                confirmDismiss: (direction) => _confirmDelete(device),
                onDismissed: (_) => _deleteDevice(device),
                background: Container(
                  margin: const EdgeInsets.only(bottom: 12),
                  decoration: BoxDecoration(
                    color: AppColors.error,
                    borderRadius: BorderRadius.circular(16),
                  ),
                  alignment: Alignment.centerRight,
                  padding: const EdgeInsets.only(right: 24),
                  child: Column(
                    mainAxisSize: MainAxisSize.min,
                    children: [
                      const Icon(
                        Icons.delete_outline,
                        color: Colors.white,
                        size: 28,
                      ),
                      const SizedBox(height: 4),
                      Text(
                        l10n?.myDevicesDelete ?? 'Delete',
                        style: const TextStyle(
                          color: Colors.white,
                          fontWeight: FontWeight.w600,
                        ),
                      ),
                    ],
                  ),
                ),
                child: _DeviceCard(
                  device: device,
                  isOnline: _deviceAvailability[device.deviceId],
                  isCheckingAvailability: _checkingAvailability,
                  onTap: () => _handleDeviceTap(device),
                  onLongPress: () => _showDeviceMenu(device),
                ),
              ),
            ),

          const SizedBox(height: 24),

          // ========== DISCOVERED DEVICES SECTION ==========
          _buildSectionHeader(
            icon: Icons.wifi_find,
            title: l10n?.myDevicesDiscovered ?? 'Discovered on Network',
            count: _discoveredDevices.length,
            color: AppColors.info,
            isLoading: _scanningNetwork,
          ),
          const SizedBox(height: 12),
          if (_scanningNetwork && _discoveredDevices.isEmpty)
            _buildScanningIndicator()
          else if (_discoveredDevices.isEmpty)
            _buildEmptySectionMessage(
              l10n?.myDevicesNoDiscovered ?? 'No new devices found',
              l10n?.myDevicesNoDiscoveredSubtitle ??
                  'Make sure devices are powered on and connected to the same WiFi',
            )
          else
            ..._discoveredDevices.map(
              (device) => _DiscoveredDeviceCard(
                device: device,
                onSave: () => _saveDiscoveredDevice(device),
              ),
            ),

          // Bottom padding for FAB
          const SizedBox(height: 80),
        ],
      ),
    );
  }

  Widget _buildSectionHeader({
    required IconData icon,
    required String title,
    required int count,
    required Color color,
    required bool isLoading,
  }) {
    return Row(
      children: [
        Container(
          padding: const EdgeInsets.all(8),
          decoration: BoxDecoration(
            color: color.withValues(alpha: 0.15),
            borderRadius: BorderRadius.circular(10),
          ),
          child: Icon(icon, color: color, size: 20),
        ),
        const SizedBox(width: 12),
        Text(
          title,
          style: Theme.of(
            context,
          ).textTheme.titleMedium?.copyWith(fontWeight: FontWeight.bold),
        ),
        const SizedBox(width: 8),
        Container(
          padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 2),
          decoration: BoxDecoration(
            color: color.withValues(alpha: 0.15),
            borderRadius: BorderRadius.circular(12),
          ),
          child: Text(
            '$count',
            style: TextStyle(
              color: color,
              fontWeight: FontWeight.bold,
              fontSize: 12,
            ),
          ),
        ),
        const Spacer(),
        if (isLoading)
          SizedBox(
            width: 16,
            height: 16,
            child: CircularProgressIndicator(
              strokeWidth: 2,
              valueColor: AlwaysStoppedAnimation<Color>(color),
            ),
          ),
      ],
    );
  }

  Widget _buildEmptySectionMessage(String title, String subtitle) {
    return Container(
      padding: const EdgeInsets.all(20),
      margin: const EdgeInsets.only(bottom: 12),
      decoration: BoxDecoration(
        color: Theme.of(context).cardColor,
        borderRadius: BorderRadius.circular(16),
        border: Border.all(color: Colors.grey.withValues(alpha: 0.2)),
      ),
      child: Column(
        children: [
          Text(
            title,
            style: TextStyle(
              color: Colors.grey[600],
              fontWeight: FontWeight.w500,
            ),
          ),
          const SizedBox(height: 4),
          Text(
            subtitle,
            textAlign: TextAlign.center,
            style: TextStyle(color: Colors.grey[500], fontSize: 12),
          ),
        ],
      ),
    );
  }

  Widget _buildScanningIndicator() {
    return Container(
      padding: const EdgeInsets.all(24),
      margin: const EdgeInsets.only(bottom: 12),
      decoration: BoxDecoration(
        color: Theme.of(context).cardColor,
        borderRadius: BorderRadius.circular(16),
        border: Border.all(color: AppColors.info.withValues(alpha: 0.3)),
      ),
      child: Column(
        children: [
          SizedBox(
            width: 32,
            height: 32,
            child: CircularProgressIndicator(
              strokeWidth: 3,
              valueColor: AlwaysStoppedAnimation<Color>(AppColors.info),
            ),
          ),
          const SizedBox(height: 12),
          Text(
            AppLocalizations.of(context)?.myDevicesScanning ??
                'Scanning network...',
            style: TextStyle(
              color: AppColors.info,
              fontWeight: FontWeight.w500,
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildEmptyState() {
    return Center(
      child: Padding(
        padding: const EdgeInsets.all(32.0),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Container(
              padding: const EdgeInsets.all(32),
              decoration: BoxDecoration(
                shape: BoxShape.circle,
                color: AppColors.forestGreen.withValues(alpha: 0.1),
              ),
              child: const Icon(
                Icons.park,
                size: 80,
                color: AppColors.forestGreen,
              ),
            ),
            const SizedBox(height: 32),
            Text(
              AppLocalizations.of(context)?.myDevicesNoDevicesYet ??
                  'No Devices Yet',
              style: Theme.of(context).textTheme.headlineMedium,
            ),
            const SizedBox(height: 12),
            Text(
              AppLocalizations.of(context)?.myDevicesNoDevicesSubtitle ??
                  'Add your first PixelTree device\nto get started',
              textAlign: TextAlign.center,
              style: Theme.of(
                context,
              ).textTheme.bodyLarge?.copyWith(color: Colors.grey[600]),
            ),
            const SizedBox(height: 32),
            ElevatedButton.icon(
              onPressed: () {
                Navigator.push(
                  context,
                  MaterialPageRoute(
                    builder: (_) => const ConnectionModeScreen(),
                  ),
                ).then((_) => _loadDevices());
              },
              icon: const Icon(Icons.add),
              label: Text(
                AppLocalizations.of(context)?.myDevicesAddDevice ??
                    'Add Device',
              ),
            ),
          ],
        ),
      ),
    );
  }

  void _showDeviceMenu(SavedDevice device) {
    showModalBottomSheet(
      context: context,
      shape: const RoundedRectangleBorder(
        borderRadius: BorderRadius.vertical(top: Radius.circular(20)),
      ),
      builder: (ctx) => SafeArea(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            // Handle bar
            Container(
              margin: const EdgeInsets.only(top: 12, bottom: 8),
              width: 48,
              height: 4,
              decoration: BoxDecoration(
                color: Colors.grey[300],
                borderRadius: BorderRadius.circular(2),
              ),
            ),

            // Device name
            Padding(
              padding: const EdgeInsets.all(16.0),
              child: Text(
                device.name,
                style: Theme.of(context).textTheme.titleLarge,
              ),
            ),

            const Divider(height: 1),

            // Delete option
            ListTile(
              leading: const Icon(Icons.delete_outline, color: AppColors.error),
              title: Text(
                AppLocalizations.of(context)?.myDevicesRemoveDevice ??
                    'Remove Device',
                style: const TextStyle(color: AppColors.error),
              ),
              onTap: () async {
                Navigator.pop(ctx); // Close bottom sheet
                final confirmed = await _confirmDelete(device);
                if (confirmed == true) {
                  _deleteDevice(device);
                }
              },
            ),

            // Cancel
            ListTile(
              leading: const Icon(Icons.close),
              title: Text(
                AppLocalizations.of(context)?.buttonCancel ?? 'Cancel',
              ),
              onTap: () => Navigator.pop(ctx),
            ),

            const SizedBox(height: 8),
          ],
        ),
      ),
    );
  }

  Future<bool?> _confirmDelete(SavedDevice device) {
    return showDialog<bool>(
      context: context,
      builder: (ctx) => AlertDialog(
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
        title: Text(
          AppLocalizations.of(context)?.myDevicesRemoveTitle ??
              'Remove Device?',
        ),
        content: Text(
          AppLocalizations.of(context)?.myDevicesRemoveMessage(device.name) ??
              'Remove ${device.name} from your saved devices?',
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.pop(ctx, false),
            child: Text(AppLocalizations.of(context)?.buttonCancel ?? 'Cancel'),
          ),
          TextButton(
            onPressed: () => Navigator.pop(ctx, true),
            style: TextButton.styleFrom(foregroundColor: AppColors.error),
            child: Text(
              AppLocalizations.of(context)?.myDevicesRemoveDevice ?? 'Remove',
            ),
          ),
        ],
      ),
    );
  }

  Future<void> _deleteDevice(SavedDevice device) async {
    await _repository.removeDevice(device.deviceId);
    if (mounted) {
      setState(() {
        _devices.removeWhere((d) => d.deviceId == device.deviceId);
      });

      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text(
            AppLocalizations.of(context)?.myDevicesRemoved(device.name) ??
                '${device.name} removed',
          ),
          behavior: SnackBarBehavior.floating,
          shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(8)),
        ),
      );
    }
  }

  void _handleDeviceTap(SavedDevice device) {
    final isOnline = _deviceAvailability[device.deviceId];

    // Don't allow tap during availability check (initial load OR manual refresh)
    if (_checkingAvailability) {
      return;
    }

    // Show offline tips dialog
    if (isOnline == false) {
      _showOfflineDialog(device);
      return;
    }

    // Connect to online device
    _connectToDevice(device);
  }

  void _showOfflineDialog(SavedDevice device) {
    showDialog(
      context: context,
      builder: (ctx) => AlertDialog(
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
        title: Row(
          children: [
            const Icon(Icons.wifi_off, color: Colors.orange),
            const SizedBox(width: 8),
            Flexible(
              child: Text(
                AppLocalizations.of(
                      context,
                    )?.myDevicesDeviceOffline(device.name) ??
                    '${device.name} Offline',
                overflow: TextOverflow.ellipsis,
              ),
            ),
          ],
        ),
        content: Column(
          mainAxisSize: MainAxisSize.min,
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(
              AppLocalizations.of(context)?.myDevicesOfflineMessage ??
                  'Device is not reachable on the network.',
              style: const TextStyle(fontWeight: FontWeight.w500),
            ),
            const SizedBox(height: 16),
            Text(
              AppLocalizations.of(context)?.myDevicesTroubleshooting ??
                  'Troubleshooting tips:',
            ),
            const SizedBox(height: 8),
            _buildTipRow(
              AppLocalizations.of(context)?.myDevicesTip1 ??
                  '• Check device is powered on',
            ),
            _buildTipRow(
              AppLocalizations.of(context)?.myDevicesTip2 ??
                  '• Verify device is connected to WiFi',
            ),
            _buildTipRow(
              AppLocalizations.of(context)?.myDevicesTip3 ??
                  '• Ensure phone is on same network',
            ),
            _buildTipRow(
              AppLocalizations.of(context)?.myDevicesTip4 ??
                  '• Try refreshing device status',
            ),
          ],
        ),
        actions: [
          TextButton(
            onPressed: () {
              Navigator.pop(ctx);
              _checkDevicesAvailability();
            },
            child: Text(
              AppLocalizations.of(context)?.buttonRefresh ?? 'Refresh',
            ),
          ),
          TextButton(
            onPressed: () => Navigator.pop(ctx),
            child: Text(AppLocalizations.of(context)?.buttonOk ?? 'OK'),
          ),
        ],
      ),
    );
  }

  Widget _buildTipRow(String text) {
    return Padding(
      padding: const EdgeInsets.only(bottom: 4),
      child: Text(
        text,
        style: TextStyle(fontSize: 13, color: Colors.grey[600]),
      ),
    );
  }

  Future<void> _connectToDevice(SavedDevice device) async {
    // Show loading
    showDialog(
      context: context,
      barrierDismissible: false,
      builder: (ctx) => Center(
        child: Card(
          child: Padding(
            padding: const EdgeInsets.all(32.0),
            child: Column(
              mainAxisSize: MainAxisSize.min,
              children: [
                const CircularProgressIndicator(),
                const SizedBox(height: 16),
                Text(
                  AppLocalizations.of(context)?.myDevicesSearching ??
                      'Searching for device...',
                ),
              ],
            ),
          ),
        ),
      ),
    );

    try {
      // Create HTTP datasource and discovery service
      final httpDataSource = HttpDataSource();
      final discoveryService = DeviceDiscoveryService(httpDataSource);

      // Try to discover device on network
      final result = await discoveryService.discoverDevice(device.deviceId);

      if (mounted) {
        Navigator.pop(context); // Close loading

        if (result != null) {
          // Device found!
          final status = result['status'] as Map<String, dynamic>;
          final ipAddress = status['ip'] as String?;
          final wifiSSID = status['ssid'] as String?;

          // Update SavedDevice with new info
          final updatedDevice = device.copyWith(
            lastSeen: DateTime.now(),
            ipAddress: ipAddress,
            wifiSSID: wifiSSID,
          );
          await _repository.saveDevice(updatedDevice);

          // Refresh device list to show updated info
          await _loadDevices();

          // Navigate to device connected screen
          if (mounted) {
            Navigator.of(context).push(
              MaterialPageRoute(
                builder: (_) => DeviceConnectedScreen(
                  connectionType: 'WiFi',
                  deviceIp: ipAddress,
                ),
              ),
            );
          }
        } else {
          // Device not found - show error
          _showError(
            AppLocalizations.of(context)?.myDevicesNotFound ??
                'Device not found on network.\n\nMake sure:\n'
                    '• Device is powered on\n'
                    '• Device is connected to WiFi\n'
                    '• Phone is on same network',
          );
        }
      }

      // Cleanup
      httpDataSource.dispose();
    } catch (e) {
      if (mounted) {
        Navigator.pop(context); // Close loading
        _showError('Error: ${e.toString()}');
      }
    }
  }

  void _showError(String message) {
    showDialog(
      context: context,
      builder: (ctx) => AlertDialog(
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
        title: Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            const Icon(Icons.error_outline, color: AppColors.error),
            const SizedBox(width: 8),
            Flexible(
              child: Text(
                AppLocalizations.of(context)?.provisioningFailedTitle ??
                    'Connection Failed',
                overflow: TextOverflow.ellipsis,
              ),
            ),
          ],
        ),
        content: Text(message),
        actions: [
          TextButton(
            onPressed: () => Navigator.pop(ctx),
            child: Text(AppLocalizations.of(context)?.buttonOk ?? 'OK'),
          ),
        ],
      ),
    );
  }
}

// ============================================================================
// Device Card Widget
// ============================================================================

class _DeviceCard extends StatelessWidget {
  final SavedDevice device;
  final bool? isOnline; // null = checking, true = online, false = offline
  final bool isCheckingAvailability;
  final VoidCallback onTap;
  final VoidCallback onLongPress;

  const _DeviceCard({
    required this.device,
    required this.isOnline,
    required this.isCheckingAvailability,
    required this.onTap,
    required this.onLongPress,
  });

  @override
  Widget build(BuildContext context) {
    final isDark = Theme.of(context).brightness == Brightness.dark;

    return Container(
      margin: const EdgeInsets.only(bottom: 12),
      child: Material(
        color: Colors.transparent,
        borderRadius: BorderRadius.circular(16),
        child: InkWell(
          onTap: onTap,
          onLongPress: onLongPress,
          borderRadius: BorderRadius.circular(16),
          child: Container(
            padding: const EdgeInsets.all(14),
            decoration: BoxDecoration(
              color: Theme.of(context).cardColor,
              borderRadius: BorderRadius.circular(16),
              border: Border.all(
                color: isOnline == true
                    ? AppColors.forestGreen.withValues(alpha: 0.3)
                    : (isDark
                          ? Colors.white.withValues(alpha: 0.08)
                          : Colors.black.withValues(alpha: 0.05)),
                width: 1,
              ),
              boxShadow: [
                BoxShadow(
                  color: Colors.black.withValues(alpha: isDark ? 0.2 : 0.04),
                  blurRadius: 8,
                  offset: const Offset(0, 2),
                ),
              ],
            ),
            child: Row(
              children: [
                // Compact icon with status
                Stack(
                  clipBehavior: Clip.none,
                  children: [
                    Container(
                      padding: const EdgeInsets.all(10),
                      decoration: BoxDecoration(
                        color: AppColors.forestGreen.withValues(alpha: 0.12),
                        borderRadius: BorderRadius.circular(12),
                      ),
                      child: const Icon(
                        Icons.park,
                        size: 24,
                        color: AppColors.forestGreen,
                      ),
                    ),
                    // Compact status dot
                    if (isOnline != null)
                      Positioned(
                        right: -3,
                        top: -3,
                        child: Container(
                          width: 12,
                          height: 12,
                          decoration: BoxDecoration(
                            color: isOnline! ? AppColors.success : Colors.grey,
                            shape: BoxShape.circle,
                            border: Border.all(
                              color: Theme.of(context).cardColor,
                              width: 2,
                            ),
                          ),
                        ),
                      ),
                  ],
                ),
                const SizedBox(width: 14),

                // Compact info
                Expanded(
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    mainAxisSize: MainAxisSize.min,
                    children: [
                      // Name + Badge
                      Row(
                        children: [
                          Expanded(
                            child: Text(
                              device.name,
                              style: Theme.of(context).textTheme.titleSmall
                                  ?.copyWith(fontWeight: FontWeight.w600),
                              overflow: TextOverflow.ellipsis,
                            ),
                          ),
                          // Compact badge
                          if (isCheckingAvailability && isOnline == null)
                            const SizedBox(
                              width: 12,
                              height: 12,
                              child: CircularProgressIndicator(
                                strokeWidth: 1.5,
                              ),
                            )
                          else if (isOnline != null)
                            Container(
                              padding: const EdgeInsets.symmetric(
                                horizontal: 6,
                                vertical: 2,
                              ),
                              decoration: BoxDecoration(
                                color:
                                    (isOnline!
                                            ? AppColors.success
                                            : Colors.grey)
                                        .withValues(alpha: 0.15),
                                borderRadius: BorderRadius.circular(6),
                              ),
                              child: Text(
                                isOnline!
                                    ? (AppLocalizations.of(
                                            context,
                                          )?.myDevicesOnline ??
                                          'Online')
                                    : (AppLocalizations.of(
                                            context,
                                          )?.myDevicesOffline ??
                                          'Offline'),
                                style: TextStyle(
                                  color: isOnline!
                                      ? AppColors.success
                                      : Colors.grey,
                                  fontWeight: FontWeight.w600,
                                  fontSize: 9,
                                ),
                              ),
                            ),
                        ],
                      ),
                      const SizedBox(height: 6),

                      // Compact info rows
                      Row(
                        children: [
                          if (device.wifiSSID != null)
                            Flexible(
                              child: _buildCompactInfo(
                                Icons.wifi,
                                device.wifiSSID!,
                                AppColors.forestGreen,
                              ),
                            ),

                          // Separator (dot) and IP
                          if (device.wifiSSID != null &&
                              device.ipAddress != null)
                            Padding(
                              padding: const EdgeInsets.symmetric(
                                horizontal: 6,
                              ),
                              child: Text(
                                '•',
                                style: TextStyle(
                                  color: Colors.grey.withValues(alpha: 0.5),
                                  fontSize: 10,
                                ),
                              ),
                            ),

                          if (device.ipAddress != null)
                            _buildCompactInfo(
                              Icons.lan,
                              device.ipAddress!,
                              Colors.blue,
                            ),
                        ],
                      ),

                      const SizedBox(height: 3),

                      _buildCompactInfo(
                        Icons.access_time,
                        _formatLastSeen(device.lastSeen, context),
                        Colors.grey,
                      ),
                    ],
                  ),
                ),

                // Subtle chevron
                Icon(
                  Icons.chevron_right,
                  size: 18,
                  color: Colors.grey.withValues(alpha: 0.4),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }

  Widget _buildCompactInfo(IconData icon, String text, Color color) {
    return Row(
      mainAxisSize: MainAxisSize.min,
      children: [
        Icon(icon, size: 11, color: color.withValues(alpha: 0.7)),
        const SizedBox(width: 5),
        Flexible(
          child: Text(
            text,
            style: TextStyle(
              color: color.withValues(alpha: 0.8),
              fontSize: 11,
              fontWeight: FontWeight.w500,
            ),
            overflow: TextOverflow.ellipsis,
          ),
        ),
      ],
    );
  }

  String _formatLastSeen(DateTime lastSeen, BuildContext context) {
    final diff = DateTime.now().difference(lastSeen);
    final l10n = AppLocalizations.of(context);

    if (diff.inMinutes < 1) {
      return l10n?.myDevicesLastSeenJustNow ?? 'Last seen just now';
    }
    if (diff.inMinutes < 60) {
      return l10n?.myDevicesLastSeenMinutes(diff.inMinutes) ??
          'Last seen ${diff.inMinutes} min ago';
    }
    if (diff.inHours < 24) {
      return l10n?.myDevicesLastSeenHours(diff.inHours) ??
          'Last seen ${diff.inHours} hours ago';
    }
    if (diff.inDays < 7) {
      return l10n?.myDevicesLastSeenDays(diff.inDays) ??
          'Last seen ${diff.inDays} days ago';
    }
    final weeks = (diff.inDays / 7).floor();
    return l10n?.myDevicesLastSeenWeeks(weeks) ?? 'Last seen $weeks weeks ago';
  }
}

// ============================================================================
// Discovered Device Card Widget - For Network Found Devices
// ============================================================================

class _DiscoveredDeviceCard extends StatelessWidget {
  final DiscoveredDevice device;
  final VoidCallback onSave;

  const _DiscoveredDeviceCard({required this.device, required this.onSave});

  @override
  Widget build(BuildContext context) {
    final isDark = Theme.of(context).brightness == Brightness.dark;

    return Container(
      margin: const EdgeInsets.only(bottom: 12),
      child: Material(
        color: Colors.transparent,
        borderRadius: BorderRadius.circular(16),
        child: Container(
          padding: const EdgeInsets.all(14),
          decoration: BoxDecoration(
            color: Theme.of(context).cardColor,
            borderRadius: BorderRadius.circular(16),
            border: Border.all(
              color: AppColors.info.withValues(alpha: 0.3),
              width: 1,
            ),
            boxShadow: [
              BoxShadow(
                color: Colors.black.withValues(alpha: isDark ? 0.2 : 0.04),
                blurRadius: 8,
                offset: const Offset(0, 2),
              ),
            ],
          ),
          child: Row(
            children: [
              // Icon with network indicator
              Container(
                padding: const EdgeInsets.all(10),
                decoration: BoxDecoration(
                  color: AppColors.info.withValues(alpha: 0.12),
                  borderRadius: BorderRadius.circular(12),
                ),
                child: const Icon(Icons.wifi, size: 24, color: AppColors.info),
              ),
              const SizedBox(width: 14),

              // Device info
              Expanded(
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    // Name + NEW badge
                    Row(
                      children: [
                        Expanded(
                          child: Text(
                            device.name,
                            style: Theme.of(context).textTheme.titleSmall
                                ?.copyWith(fontWeight: FontWeight.w600),
                            overflow: TextOverflow.ellipsis,
                          ),
                        ),
                        Container(
                          padding: const EdgeInsets.symmetric(
                            horizontal: 6,
                            vertical: 2,
                          ),
                          decoration: BoxDecoration(
                            color: AppColors.info.withValues(alpha: 0.15),
                            borderRadius: BorderRadius.circular(6),
                          ),
                          child: Text(
                            AppLocalizations.of(context)?.myDevicesNew ?? 'NEW',
                            style: const TextStyle(
                              color: AppColors.info,
                              fontWeight: FontWeight.bold,
                              fontSize: 9,
                            ),
                          ),
                        ),
                      ],
                    ),
                    const SizedBox(height: 6),

                    // Info rows
                    Row(
                      children: [
                        if (device.ssid != null)
                          Flexible(
                            child: _buildCompactInfo(
                              Icons.wifi,
                              device.ssid!,
                              AppColors.info,
                            ),
                          ),
                        if (device.ssid != null && device.ipAddress != null)
                          Padding(
                            padding: const EdgeInsets.symmetric(horizontal: 6),
                            child: Text(
                              '•',
                              style: TextStyle(
                                color: Colors.grey.withValues(alpha: 0.5),
                                fontSize: 10,
                              ),
                            ),
                          ),
                        if (device.ipAddress != null)
                          _buildCompactInfo(
                            Icons.lan,
                            device.ipAddress!,
                            Colors.blue,
                          ),
                      ],
                    ),
                  ],
                ),
              ),

              // Add button
              const SizedBox(width: 8),
              Material(
                color: AppColors.forestGreen,
                borderRadius: BorderRadius.circular(12),
                child: InkWell(
                  onTap: onSave,
                  borderRadius: BorderRadius.circular(12),
                  child: Container(
                    padding: const EdgeInsets.all(10),
                    child: const Icon(Icons.add, color: Colors.white, size: 20),
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }

  Widget _buildCompactInfo(IconData icon, String text, Color color) {
    return Row(
      mainAxisSize: MainAxisSize.min,
      children: [
        Icon(icon, size: 11, color: color.withValues(alpha: 0.7)),
        const SizedBox(width: 5),
        Flexible(
          child: Text(
            text,
            style: TextStyle(
              color: color.withValues(alpha: 0.8),
              fontSize: 11,
              fontWeight: FontWeight.w500,
            ),
            overflow: TextOverflow.ellipsis,
          ),
        ),
      ],
    );
  }
}
