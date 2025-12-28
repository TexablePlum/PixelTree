import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';
import '../../../core/constants/app_colors.dart';
import '../../../l10n/app_localizations.dart';
import '../../../data/models/wifi_network.dart';
import '../../../data/models/saved_device.dart';
import '../../../data/datasources/device_storage.dart';
import '../../../data/repositories/device_repository.dart';
import '../../../data/services/provisioning_service.dart';
import '../../../data/services/ble_provisioning_service.dart';
import '../../screens/my_devices/my_devices_screen.dart';

// ============================================================================
// Provisioning Wizard Screen - 4-Step WiFi Setup
// ============================================================================
// Step 1: Key Exchange (automatic for BLE, skipped for HTTP)
// Step 2: WiFi Network Selection
// Step 3: Password Entry
// Step 4: Connecting & Status

class ProvisioningWizardScreen extends StatefulWidget {
  final ProvisioningService service;

  const ProvisioningWizardScreen({super.key, required this.service});

  @override
  State<ProvisioningWizardScreen> createState() =>
      _ProvisioningWizardScreenState();
}

class _ProvisioningWizardScreenState extends State<ProvisioningWizardScreen> {
  int _currentStep = 0;
  List<WiFiNetwork> _networks = [];
  WiFiNetwork? _selectedNetwork;
  String _password = '';
  bool _obscurePassword = true;
  bool _isLoading = false;
  String? _errorMessage;

  @override
  void initState() {
    super.initState();
    // Auto-start with Step 1
    _performKeyExchange();
  }

  @override
  void dispose() {
    widget.service.dispose();
    super.dispose();
  }

  // ========================================================================
  // Step 1: Key Exchange
  // ========================================================================

  Future<void> _performKeyExchange() async {
    setState(() {
      _isLoading = true;
      _errorMessage = null;
    });

    try {
      // Initialize service (connect)
      await widget.service.initialize();

      // Perform key exchange (BLE only, HTTP returns true immediately)
      await widget.service.performKeyExchange();

      // Success - move to step 2
      setState(() {
        _currentStep = 1;
      });

      // Auto-trigger WiFi scan
      _scanNetworks();
    } catch (e) {
      setState(() {
        _errorMessage =
            AppLocalizations.of(
              context,
            )?.provisioningConnectionFailed(e.toString()) ??
            'Connection failed: ${e.toString()}';
        _isLoading = false;
      });
    }
  }

  // ========================================================================
  // Step 2: WiFi Scan
  // ========================================================================

  Future<void> _scanNetworks() async {
    setState(() {
      _isLoading = true;
      _errorMessage = null;
    });

    try {
      final networks = await widget.service.scanNetworks();

      setState(() {
        _networks = networks;
        _isLoading = false;
      });
    } catch (e) {
      setState(() {
        _errorMessage =
            AppLocalizations.of(context)?.provisioningFailedBody ??
            'Failed to scan WiFi networks';
        _isLoading = false;
      });
    }
  }

  void _selectNetwork(WiFiNetwork network) {
    setState(() {
      _selectedNetwork = network;
      _currentStep = 2; // Move to password entry
    });
  }

  // ========================================================================
  // Step 3: Password Entry
  // ========================================================================

  void _submitCredentials() {
    if (_selectedNetwork == null || _password.isEmpty) {
      return;
    }

    _provisionDevice();
  }

  // ========================================================================
  // Step 4: Provisioning
  // ========================================================================

  Future<void> _provisionDevice() async {
    setState(() {
      _currentStep = 3;
      _isLoading = true;
      _errorMessage = null;
    });

    try {
      final success = await widget.service.provisionDevice(
        _selectedNetwork!.ssid,
        _password,
      );

      if (success) {
        // Success - Navigation handled by _buildConnectingStep
        setState(() {
          _isLoading = false;
        });
      } else {
        // Generic failure - stay on step 3
        setState(() {
          _errorMessage =
              AppLocalizations.of(context)?.provisioningFailedBody ??
              'Connection failed. Please try again.';
          _isLoading = false;
        });
      }
    } on WrongPasswordException {
      // Wrong password - go back to password entry
      setState(() {
        _currentStep = 2;
        _errorMessage =
            AppLocalizations.of(context)?.provisioningWrongPassword ??
            'Wrong WiFi password. Please check and try again.';
        _isLoading = false;
        _password = ''; // Clear password field
      });
    } on SsidNotFoundException {
      // Network disappeared - go back to network selection
      setState(() {
        _currentStep = 1;
        _errorMessage =
            AppLocalizations.of(context)?.provisioningSsidNotFound ??
            'WiFi network disappeared. Please select another.';
        _isLoading = false;
      });
      _scanNetworks(); // Re-scan networks
    } on AuthFailedException {
      // Auth failed - go back to password entry
      setState(() {
        _currentStep = 2;
        _errorMessage =
            AppLocalizations.of(context)?.provisioningAuthFailed ??
            'WiFi authentication failed. Please check password.';
        _isLoading = false;
        _password = '';
      });
    } on ConnectionTimeoutException {
      // Timeout - stay on connecting screen with error
      setState(() {
        _errorMessage =
            AppLocalizations.of(context)?.provisioningTimeout ??
            'Connection timeout. Please try again.';
        _isLoading = false;
      });
    } catch (e) {
      // Unknown error
      setState(() {
        _errorMessage =
            AppLocalizations.of(context)?.provisioningError(e.toString()) ??
            'Error: ${e.toString()}';
        _isLoading = false;
      });
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
          AppLocalizations.of(context)?.provisioningTitle ??
              'WiFi Setup Wizard',
        ),
      ),
      body: Stack(
        children: [
          // Main content - resizes with keyboard
          SafeArea(
            child: Padding(
              padding: const EdgeInsets.all(24.0),
              child: Column(
                children: [
                  // Step indicator text
                  Text(
                    '${AppLocalizations.of(context)?.provisioningStep ?? 'Step'} ${_currentStep + 1} ${AppLocalizations.of(context)?.provisioningOf ?? 'of'} 4',
                    style: Theme.of(
                      context,
                    ).textTheme.bodySmall?.copyWith(color: Colors.grey[600]),
                  ),
                  const SizedBox(height: 8),

                  // Progress indicator
                  _buildProgressBar(),
                  const SizedBox(height: 32),

                  // Step content
                  Expanded(child: _buildStepContent()),

                  // Error message - shown on all steps EXCEPT step 2 (it has special handling)
                  if (_errorMessage != null && _currentStep != 2) ...[
                    Container(
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
                    const SizedBox(height: 16),
                  ],

                  // Action button
                  if (_currentStep == 2 && !_isLoading)
                    ElevatedButton(
                      onPressed: _password.isNotEmpty
                          ? _submitCredentials
                          : null,
                      style: ElevatedButton.styleFrom(
                        minimumSize: const Size(double.infinity, 56),
                      ),
                      child: Text(
                        AppLocalizations.of(context)?.buttonConnect ??
                            'Connect',
                      ),
                    ),
                ],
              ),
            ),
          ),

          // Error banner for step 2 - hidden when keyboard is active
          if (_errorMessage != null &&
              _currentStep == 2 &&
              MediaQuery.of(context).viewInsets.bottom ==
                  0) // Hide when keyboard visible
            Positioned(
              left: 0,
              right: 0,
              bottom: 75, // Above the Connect button
              child: Container(
                padding: const EdgeInsets.all(16),
                margin: const EdgeInsets.all(24),
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
            ),
        ],
      ),
    );
  }

  Widget _buildProgressBar() {
    return Row(
      children: List.generate(4, (index) {
        final isActive = index <= _currentStep;
        final isCompleted = index < _currentStep;

        return Expanded(
          child: Container(
            height: 4,
            margin: EdgeInsets.only(right: index < 3 ? 8 : 0),
            decoration: BoxDecoration(
              color: isCompleted
                  ? AppColors.success
                  : isActive
                  ? AppColors.forestGreen
                  : Colors.grey[300],
              borderRadius: BorderRadius.circular(2),
            ),
          ),
        );
      }),
    );
  }

  Widget _buildStepContent() {
    switch (_currentStep) {
      case 0:
        return _buildKeyExchangeStep();
      case 1:
        return _buildNetworkSelectionStep();
      case 2:
        return _buildPasswordEntryStep();
      case 3:
        return _buildConnectingStep();
      default:
        return const SizedBox();
    }
  }

  // Step 1: Key Exchange
  Widget _buildKeyExchangeStep() {
    return Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          const CircularProgressIndicator(),
          const SizedBox(height: 24),
          Text(
            AppLocalizations.of(context)?.provisioningKeyExchangeTitle ??
                'Establishing Secure Connection...',
            style: Theme.of(context).textTheme.headlineSmall,
            textAlign: TextAlign.center,
          ),
          const SizedBox(height: 12),
          Text(
            AppLocalizations.of(context)?.provisioningKeyExchangeBody ??
                'Exchanging encryption keys...',
            style: Theme.of(context).textTheme.bodyMedium,
            textAlign: TextAlign.center,
          ),
        ],
      ),
    );
  }

  // Step 2: Network Selection
  Widget _buildNetworkSelectionStep() {
    if (_isLoading) {
      return Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            const CircularProgressIndicator(),
            const SizedBox(height: 24),
            Text(
              AppLocalizations.of(context)?.provisioningScanningNetworks ??
                  'Scanning WiFi Networks',
              style: Theme.of(context).textTheme.headlineSmall,
              textAlign: TextAlign.center,
            ),
            const SizedBox(height: 12),
            Text(
              AppLocalizations.of(context)?.provisioningScanningMessage ??
                  'Please wait while your device scans for available networks...',
              style: Theme.of(
                context,
              ).textTheme.bodyMedium?.copyWith(color: Colors.grey[600]),
              textAlign: TextAlign.center,
            ),
          ],
        ),
      );
    }

    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text(
          AppLocalizations.of(context)?.provisioningSelectNetworkTitle ??
              'Select Your WiFi Network',
          style: Theme.of(context).textTheme.headlineSmall,
        ),
        const SizedBox(height: 16),
        Expanded(
          child: ListView.builder(
            itemCount: _networks.length,
            itemBuilder: (context, index) {
              final network = _networks[index];
              return Card(
                margin: const EdgeInsets.only(bottom: 8),
                child: ListTile(
                  leading: Icon(
                    network.secure ? Icons.lock : Icons.lock_open,
                    color: network.secure ? AppColors.forestGreen : Colors.grey,
                  ),
                  title: Text(network.ssid),
                  subtitle: Row(
                    children: [
                      ...List.generate(
                        4,
                        (i) => Icon(
                          Icons.signal_cellular_alt,
                          size: 12,
                          color: i < network.getSignalBars()
                              ? AppColors.success
                              : Colors.grey[300],
                        ),
                      ),
                      const SizedBox(width: 8),
                      Text('${network.rssi} dBm'),
                    ],
                  ),
                  onTap: () => _selectNetwork(network),
                ),
              );
            },
          ),
        ),
      ],
    );
  }

  // Step 3: Password Entry
  Widget _buildPasswordEntryStep() {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text(
          AppLocalizations.of(context)?.provisioningEnterPasswordTitle ??
              'Enter WiFi Password',
          style: Theme.of(context).textTheme.headlineSmall,
        ),
        const SizedBox(height: 8),
        Text(
          AppLocalizations.of(
                context,
              )?.provisioningNetworkLabel(_selectedNetwork?.ssid ?? '') ??
              'Network: ${_selectedNetwork?.ssid}',
          style: Theme.of(context).textTheme.bodyMedium,
        ),
        const SizedBox(height: 24),
        TextField(
          obscureText: _obscurePassword,
          onChanged: (value) => setState(() => _password = value),
          decoration: InputDecoration(
            labelText:
                AppLocalizations.of(context)?.provisioningPasswordHint ??
                'Password...',
            suffixIcon: IconButton(
              icon: Icon(
                _obscurePassword ? Icons.visibility : Icons.visibility_off,
              ),
              onPressed: () =>
                  setState(() => _obscurePassword = !_obscurePassword),
            ),
          ),
          onSubmitted: (_) =>
              _password.isNotEmpty ? _submitCredentials() : null,
        ),
        const SizedBox(height: 16),
        Text(
          AppLocalizations.of(context)?.provisioningPasswordInfo ??
              'Your password is encrypted before being sent.',
          style: Theme.of(
            context,
          ).textTheme.bodySmall?.copyWith(color: Colors.grey[600]),
        ),
      ],
    );
  }

  // Step 4: Connecting
  Widget _buildConnectingStep() {
    final isSuccess = widget.service.state == ProvisioningState.success;

    // Save device and navigate to My Devices after success
    if (isSuccess) {
      Future.delayed(const Duration(seconds: 2), () async {
        if (mounted) {
          // Save device to local storage
          try {
            final prefs = await SharedPreferences.getInstance();
            final storage = DeviceStorage(prefs);
            final repository = DeviceRepository(storage);

            // Get device info from BLE service
            final bleService = widget.service as BLEProvisioningService;

            final savedDevice = SavedDevice(
              deviceId: bleService.device.macSuffix,
              name: bleService.device.name,
              lastSeen: DateTime.now(),
            );

            await repository.saveDevice(savedDevice);
          } catch (e) {
            // Log error but continue navigation
            debugPrint('Error saving device: $e');
          }

          // Navigate to My Devices
          if (mounted) {
            Navigator.of(context).pushAndRemoveUntil(
              MaterialPageRoute(builder: (_) => const MyDevicesScreen()),
              (route) => false, // Remove all previous routes
            );
          }
        }
      });
    }

    return Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          if (isSuccess)
            const Icon(Icons.check_circle, size: 80, color: AppColors.success)
          else
            const CircularProgressIndicator(),
          const SizedBox(height: 24),
          Text(
            isSuccess
                ? (AppLocalizations.of(context)?.provisioningSuccessTitle ??
                      'Connected Successfully!')
                : (AppLocalizations.of(context)?.provisioningConnectingTitle ??
                      'Connecting to WiFi...'),
            style: Theme.of(context).textTheme.headlineSmall,
            textAlign: TextAlign.center,
          ),
          const SizedBox(height: 12),
          if (!isSuccess)
            Text(
              AppLocalizations.of(context)?.provisioningConnectingBody ??
                  'This may take up to 30 seconds...',
              style: Theme.of(context).textTheme.bodyMedium,
              textAlign: TextAlign.center,
            ),
        ],
      ),
    );
  }
}
