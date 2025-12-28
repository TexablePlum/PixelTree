import 'package:flutter/material.dart';
import 'package:network_info_plus/network_info_plus.dart';
import 'package:permission_handler/permission_handler.dart';
import 'dart:io' show Platform;
import '../../../core/constants/app_colors.dart';
import '../../../l10n/app_localizations.dart';
import '../device_connected/device_connected_screen.dart';

// ============================================================================
// AP Connection Screen - Styled to match Connection Mode Screen
// ============================================================================

class APConnectionScreen extends StatefulWidget {
  const APConnectionScreen({super.key});

  @override
  State<APConnectionScreen> createState() => _APConnectionScreenState();
}

class _APConnectionScreenState extends State<APConnectionScreen> {
  final NetworkInfo _networkInfo = NetworkInfo();
  bool _isChecking = false;

  Future<void> _checkAndNavigate() async {
    setState(() {
      _isChecking = true;
    });

    try {
      // Request location permission on Android (required for WiFi SSID)
      if (Platform.isAndroid) {
        final status = await Permission.locationWhenInUse.request();

        if (!status.isGranted) {
          if (mounted) {
            _showPermissionDeniedDialog();
          }
          return;
        }
      }

      // Check if connected to WiFi
      final wifiName = await _networkInfo.getWifiName();

      if (wifiName != null && wifiName.contains('PixelTree')) {
        // Connected to PixelTree AP - proceed to success screen
        if (mounted) {
          Navigator.of(context).pushAndRemoveUntil(
            MaterialPageRoute(
              builder: (_) =>
                  const DeviceConnectedScreen(connectionType: 'Access Point'),
            ),
            (route) => false, // Remove all previous routes
          );
        }
      } else {
        // Not connected to PixelTree AP
        if (mounted) {
          _showNotConnectedDialog(wifiName);
        }
      }
    } catch (e) {
      if (mounted) {
        _showNotConnectedDialog(null);
      }
    } finally {
      if (mounted) {
        setState(() {
          _isChecking = false;
        });
      }
    }
  }

  void _showNotConnectedDialog(String? currentWifi) {
    final l10n = AppLocalizations.of(context);
    showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: Text(l10n?.apConnectionNotConnected ?? 'Not Connected'),
        content: Text(
          currentWifi != null
              ? l10n?.apConnectionNotConnectedMessage(currentWifi) ??
                    'You are connected to $currentWifi.\n\nPlease connect to the PixelTree access point first.'
              : l10n?.apConnectionNotConnectedNoWifi ??
                    'Please connect to the PixelTree access point first.',
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(context).pop(),
            child: Text(l10n?.buttonOk ?? 'OK'),
          ),
        ],
      ),
    );
  }

  void _showPermissionDeniedDialog() {
    final l10n = AppLocalizations.of(context);
    showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: Text(
          l10n?.apConnectionPermissionRequired ?? 'Permission Required',
        ),
        content: Text(
          l10n?.apConnectionPermissionMessage ??
              'Location permission is required to detect WiFi network.\n\nPlease grant permission in Settings.',
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(context).pop(),
            child: Text(l10n?.buttonCancel ?? 'Cancel'),
          ),
          TextButton(
            onPressed: () {
              Navigator.of(context).pop();
              openAppSettings();
            },
            child: Text(l10n?.apConnectionOpenSettings ?? 'Open Settings'),
          ),
        ],
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    final l10n = AppLocalizations.of(context);
    return Scaffold(
      body: Container(
        decoration: const BoxDecoration(
          gradient: LinearGradient(
            begin: Alignment.topLeft,
            end: Alignment.bottomRight,
            colors: [AppColors.forestGreen, AppColors.info],
          ),
        ),
        child: SafeArea(
          child: Padding(
            padding: const EdgeInsets.symmetric(
              horizontal: 24.0,
              vertical: 20.0,
            ),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                // Back button
                Align(
                  alignment: Alignment.centerLeft,
                  child: IconButton(
                    icon: const Icon(Icons.arrow_back, color: Colors.white),
                    onPressed: () => Navigator.of(context).pop(),
                  ),
                ),

                // Header - compact
                const Icon(Icons.wifi_tethering, size: 52, color: Colors.white),
                const SizedBox(height: 10),

                Text(
                  l10n?.apConnectionTitle ?? 'Connect to Access Point',
                  style: Theme.of(context).textTheme.headlineMedium?.copyWith(
                    color: Colors.white,
                    fontWeight: FontWeight.bold,
                    fontSize: 22,
                  ),
                  textAlign: TextAlign.center,
                ),
                const SizedBox(height: 6),

                Text(
                  l10n?.apConnectionSubtitle ?? 'Follow these steps to connect',
                  style: Theme.of(context).textTheme.bodyMedium?.copyWith(
                    color: Colors.white.withValues(alpha: 0.85),
                    fontSize: 14,
                  ),
                  textAlign: TextAlign.center,
                ),

                const SizedBox(height: 24),

                // Steps - Column instead of ListView
                _buildStepCard(
                  context,
                  number: 1,
                  title: l10n?.apConnectionStep1Title ?? 'Open WiFi Settings',
                  description:
                      l10n?.apConnectionStep1Desc ??
                      "Go to your phone's WiFi settings",
                  icon: Icons.settings,
                ),
                const SizedBox(height: 10),

                _buildStepCard(
                  context,
                  number: 2,
                  title:
                      l10n?.apConnectionStep2Title ?? 'Find PixelTree Network',
                  description:
                      l10n?.apConnectionStep2Desc ??
                      'Look for "PixelTree-XXXX"',
                  icon: Icons.search,
                ),
                const SizedBox(height: 10),

                _buildStepCard(
                  context,
                  number: 3,
                  title: l10n?.apConnectionStep3Title ?? 'Connect',
                  description:
                      l10n?.apConnectionStep3Desc ??
                      'Tap to connect (no password)',
                  icon: Icons.wifi,
                ),
                const SizedBox(height: 10),

                _buildStepCard(
                  context,
                  number: 4,
                  title: l10n?.apConnectionStep4Title ?? 'Return Here',
                  description:
                      l10n?.apConnectionStep4Desc ??
                      'Come back and tap "Continue"',
                  icon: Icons.arrow_back,
                ),

                const SizedBox(height: 16),

                // Info box - compact
                Container(
                  padding: const EdgeInsets.all(12),
                  decoration: BoxDecoration(
                    color: Colors.white.withValues(alpha: 0.2),
                    borderRadius: BorderRadius.circular(12),
                    border: Border.all(
                      color: Colors.white.withValues(alpha: 0.3),
                    ),
                  ),
                  child: Row(
                    children: [
                      Icon(Icons.info_outline, color: Colors.white, size: 18),
                      const SizedBox(width: 10),
                      Expanded(
                        child: Text(
                          l10n?.apConnectionInfo ??
                              'Phone will temporarily disconnect from internet',
                          style: Theme.of(context).textTheme.bodySmall
                              ?.copyWith(
                                color: Colors.white.withValues(alpha: 0.9),
                                fontSize: 12,
                              ),
                        ),
                      ),
                    ],
                  ),
                ),

                const Spacer(),

                // Continue button with validation
                ElevatedButton(
                  onPressed: _isChecking ? null : _checkAndNavigate,
                  style: ElevatedButton.styleFrom(
                    minimumSize: const Size(double.infinity, 54),
                    backgroundColor: Colors.white,
                    foregroundColor: AppColors.forestGreen,
                    disabledBackgroundColor: Colors.white.withValues(
                      alpha: 0.7,
                    ),
                    shape: RoundedRectangleBorder(
                      borderRadius: BorderRadius.circular(16),
                    ),
                    elevation: 0,
                  ),
                  child: _isChecking
                      ? const SizedBox(
                          height: 20,
                          width: 20,
                          child: CircularProgressIndicator(
                            strokeWidth: 2,
                            valueColor: AlwaysStoppedAnimation<Color>(
                              AppColors.forestGreen,
                            ),
                          ),
                        )
                      : Row(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            Text(
                              l10n?.apConnectionButton ?? "I'm Connected",
                              style: const TextStyle(
                                fontSize: 16,
                                fontWeight: FontWeight.w600,
                              ),
                            ),
                            const SizedBox(width: 8),
                            const Icon(Icons.arrow_forward, size: 20),
                          ],
                        ),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }

  Widget _buildStepCard(
    BuildContext context, {
    required int number,
    required String title,
    required String description,
    required IconData icon,
  }) {
    return Container(
      padding: const EdgeInsets.all(14),
      decoration: BoxDecoration(
        color: Colors.white,
        borderRadius: BorderRadius.circular(14),
        boxShadow: [
          BoxShadow(
            color: Colors.black.withValues(alpha: 0.1),
            blurRadius: 8,
            offset: const Offset(0, 3),
          ),
        ],
      ),
      child: Row(
        children: [
          // Number badge
          Container(
            width: 36,
            height: 36,
            decoration: BoxDecoration(
              color: AppColors.forestGreen.withValues(alpha: 0.1),
              borderRadius: BorderRadius.circular(10),
            ),
            child: Center(
              child: Text(
                '$number',
                style: const TextStyle(
                  color: AppColors.forestGreen,
                  fontSize: 16,
                  fontWeight: FontWeight.bold,
                ),
              ),
            ),
          ),
          const SizedBox(width: 12),

          // Icon
          Icon(icon, color: AppColors.forestGreen, size: 22),
          const SizedBox(width: 10),

          // Content
          Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  title,
                  style: Theme.of(context).textTheme.titleSmall?.copyWith(
                    fontWeight: FontWeight.w600,
                    fontSize: 14,
                    color: Colors.grey[900],
                  ),
                ),
                Text(
                  description,
                  style: Theme.of(context).textTheme.bodySmall?.copyWith(
                    color: Colors.grey[700],
                    fontSize: 12,
                  ),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}
