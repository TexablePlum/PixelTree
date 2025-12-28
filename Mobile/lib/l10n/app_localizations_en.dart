// ignore: unused_import
import 'package:intl/intl.dart' as intl;
import 'app_localizations.dart';

// ignore_for_file: type=lint

/// The translations for English (`en`).
class AppLocalizationsEn extends AppLocalizations {
  AppLocalizationsEn([String locale = 'en']) : super(locale);

  @override
  String get appName => 'PixelTree';

  @override
  String get appTagline => 'Smart ARGB Christmas Lights';

  @override
  String get onboardingWelcomeTitle => 'Welcome to PixelTree! 🎄';

  @override
  String get onboardingWelcomeBody =>
      'Transform your Christmas with smart ARGB lights';

  @override
  String get onboardingSetupTitle => 'Simple WiFi Setup';

  @override
  String get onboardingSetupBody =>
      'Connect your lights to WiFi in just a few taps';

  @override
  String get onboardingControlTitle => 'Endless Possibilities';

  @override
  String get onboardingControlBody =>
      'Choose colors, effects, and create magic';

  @override
  String get onboardingGetStarted => 'Let\'s Get Started!';

  @override
  String get onboardingDontShowAgain => 'Don\'t show this again';

  @override
  String get connectionModeTitle => 'WiFi Setup Method';

  @override
  String get connectionModeSubtitle => 'Choose how to configure your PixelTree';

  @override
  String get connectionRecommended => 'RECOMMENDED';

  @override
  String get connectionBleWifiTitle => 'BLE + WiFi';

  @override
  String get connectionBleWifiSubtitle => 'Bluetooth Low Energy';

  @override
  String get connectionBleWifiDescription =>
      'Connect via Bluetooth and configure WiFi settings. Your PixelTree will then join your WiFi network.';

  @override
  String get connectionBleWifiButton => 'Start Setup';

  @override
  String get connectionApTitle => 'Access Point Mode';

  @override
  String get connectionApDescription => 'Direct WiFi connection to PixelTree';

  @override
  String get connectionModeHelp =>
      'Not sure? Use the recommended method for easiest setup.';

  @override
  String get connectionAlreadyHaveDevices => 'Already have devices set up?';

  @override
  String get apConnectionTitle => 'Connect to Access Point';

  @override
  String get apConnectionSubtitle => 'Follow these steps to connect';

  @override
  String get apConnectionStep1Title => 'Open WiFi Settings';

  @override
  String get apConnectionStep1Desc => 'Go to your phone\'s WiFi settings';

  @override
  String get apConnectionStep2Title => 'Find PixelTree Network';

  @override
  String get apConnectionStep2Desc => 'Look for \"PixelTree-XXXX\"';

  @override
  String get apConnectionStep3Title => 'Connect';

  @override
  String get apConnectionStep3Desc => 'Tap to connect (no password)';

  @override
  String get apConnectionStep4Title => 'Return Here';

  @override
  String get apConnectionStep4Desc => 'Come back and tap \"Continue\"';

  @override
  String get apConnectionInfo =>
      'Phone will temporarily disconnect from internet';

  @override
  String get apConnectionButton => 'I\'m Connected';

  @override
  String get apConnectionNotConnected => 'Not Connected';

  @override
  String apConnectionNotConnectedMessage(String wifiName) {
    return 'You are connected to $wifiName.\n\nPlease connect to the PixelTree access point first.';
  }

  @override
  String get apConnectionNotConnectedNoWifi =>
      'Please connect to the PixelTree access point first.';

  @override
  String get apConnectionPermissionRequired => 'Permission Required';

  @override
  String get apConnectionPermissionMessage =>
      'Location permission is required to detect WiFi network.\n\nPlease grant permission in Settings.';

  @override
  String get apConnectionOpenSettings => 'Open Settings';

  @override
  String get scanTitle => 'Find Your PixelTree';

  @override
  String get scanScanning => 'Searching for devices...';

  @override
  String get scanNoDevices =>
      'No devices found. Make sure your PixelTree is powered on.';

  @override
  String get scanRefresh => 'Refresh Scan';

  @override
  String get scanConnect => 'Connect';

  @override
  String get scanConfigured => 'Configured';

  @override
  String get scanReadyToConnect => 'Ready to connect';

  @override
  String get provisioningTitle => 'WiFi Setup Wizard';

  @override
  String get provisioningStep => 'Step';

  @override
  String get provisioningOf => 'of';

  @override
  String get provisioningKeyExchangeTitle =>
      'Establishing Secure Connection...';

  @override
  String get provisioningKeyExchangeBody =>
      'Exchanging encryption keys...\nThis keeps your WiFi password safe during transfer.';

  @override
  String get provisioningSelectNetworkTitle => 'Select Your WiFi Network';

  @override
  String get provisioningScanningNetworks => 'Scanning WiFi Networks';

  @override
  String get provisioningScanningMessage =>
      'Please wait while your device scans for available networks...';

  @override
  String get provisioningEnterPasswordTitle => 'Enter WiFi Password';

  @override
  String get provisioningPasswordHint => 'Password...';

  @override
  String get provisioningPasswordInfo =>
      'Your password is encrypted before being sent to the device using AES-128.';

  @override
  String provisioningNetworkLabel(String ssid) {
    return 'Network: $ssid';
  }

  @override
  String get provisioningConnectingTitle => 'Connecting to WiFi...';

  @override
  String get provisioningConnectingBody =>
      'PixelTree is connecting to your network.\n\nThis may take up to 30 seconds...';

  @override
  String get provisioningSuccessTitle => 'Connected Successfully!';

  @override
  String get provisioningFailedTitle => 'Connection Failed';

  @override
  String get provisioningFailedBody =>
      'Could not connect to WiFi. Please check your password and try again.';

  @override
  String provisioningConnectionFailed(String error) {
    return 'Connection failed: $error';
  }

  @override
  String get provisioningWrongPassword =>
      'Wrong WiFi password. Please check and try again.';

  @override
  String get provisioningSsidNotFound =>
      'WiFi network disappeared. Please select another.';

  @override
  String get provisioningAuthFailed =>
      'WiFi authentication failed. Please check password.';

  @override
  String get provisioningTimeout => 'Connection timeout. Please try again.';

  @override
  String provisioningError(String error) {
    return 'Error: $error';
  }

  @override
  String get buttonContinue => 'Continue';

  @override
  String get buttonBack => 'Back';

  @override
  String get buttonRetry => 'Retry';

  @override
  String get buttonSkip => 'Skip';

  @override
  String get buttonConnect => 'Connect';

  @override
  String get buttonCancel => 'Cancel';

  @override
  String get buttonOk => 'OK';

  @override
  String get buttonRefresh => 'Refresh';

  @override
  String get myDevicesTitle => 'My Devices';

  @override
  String get myDevicesAddDevice => 'Add Device';

  @override
  String get myDevicesSavedDevices => 'Saved Devices';

  @override
  String get myDevicesDiscovered => 'Discovered on Network';

  @override
  String get myDevicesNoSaved => 'No saved devices yet';

  @override
  String get myDevicesNoSavedSubtitle =>
      'Add devices via BLE or save discovered ones below';

  @override
  String get myDevicesNoDiscovered => 'No new devices found';

  @override
  String get myDevicesNoDiscoveredSubtitle =>
      'Make sure devices are powered on and connected to the same WiFi';

  @override
  String get myDevicesScanning => 'Scanning network...';

  @override
  String get myDevicesNoDevicesYet => 'No Devices Yet';

  @override
  String get myDevicesNoDevicesSubtitle =>
      'Add your first PixelTree device\nto get started';

  @override
  String get myDevicesRemoveDevice => 'Remove Device';

  @override
  String get myDevicesRemoveTitle => 'Remove Device?';

  @override
  String myDevicesRemoveMessage(String deviceName) {
    return 'Remove $deviceName from your saved devices?';
  }

  @override
  String myDevicesRemoved(String deviceName) {
    return '$deviceName removed';
  }

  @override
  String myDevicesSaved(String deviceName) {
    return '$deviceName saved!';
  }

  @override
  String myDevicesOfflineTitle(String deviceName) {
    return '$deviceName Offline';
  }

  @override
  String get myDevicesOfflineMessage =>
      'Device is not reachable on the network.';

  @override
  String get myDevicesTroubleshooting => 'Troubleshooting tips:';

  @override
  String get myDevicesTip1 => '• Check device is powered on';

  @override
  String get myDevicesTip2 => '• Verify device is connected to WiFi';

  @override
  String get myDevicesTip3 => '• Ensure phone is on same network';

  @override
  String get myDevicesTip4 => '• Try refreshing device status';

  @override
  String get myDevicesSearching => 'Searching for device...';

  @override
  String get myDevicesNotFound =>
      'Device not found on network.\n\nMake sure:\n• Device is powered on\n• Device is connected to WiFi\n• Phone is on same network';

  @override
  String get myDevicesOnline => 'Online';

  @override
  String get myDevicesOffline => 'Offline';

  @override
  String get myDevicesScanningBtn => 'Scanning...';

  @override
  String get myDevicesCheckingBtn => 'Checking...';

  @override
  String get myDevicesRefreshBtn => 'Refresh';

  @override
  String get myDevicesDelete => 'Delete';

  @override
  String get myDevicesNew => 'NEW';

  @override
  String myDevicesDeviceOffline(String deviceName) {
    return '$deviceName Offline';
  }

  @override
  String get myDevicesLastSeenJustNow => 'Last seen just now';

  @override
  String myDevicesLastSeenMinutes(int minutes) {
    return 'Last seen $minutes min ago';
  }

  @override
  String myDevicesLastSeenHours(int hours) {
    return 'Last seen $hours hours ago';
  }

  @override
  String myDevicesLastSeenDays(int days) {
    return 'Last seen $days days ago';
  }

  @override
  String myDevicesLastSeenWeeks(int weeks) {
    return 'Last seen $weeks weeks ago';
  }

  @override
  String get ledControlNoConnection => 'No Connection';

  @override
  String get ledControlTryAgain => 'Try Again';

  @override
  String get dashboardTitle => 'LED Control';

  @override
  String get dashboardConnected => 'Connected';

  @override
  String get dashboardDisconnected => 'Disconnected';

  @override
  String get dashboardColor => 'Color';

  @override
  String get dashboardBrightness => 'Brightness';

  @override
  String get dashboardEffects => 'Effects';

  @override
  String get dashboardApply => 'Apply Changes';

  @override
  String get effectSolid => 'Solid';

  @override
  String get effectFade => 'Fade';

  @override
  String get effectRainbow => 'Rainbow';

  @override
  String get effectChase => 'Chase';

  @override
  String get effectTwinkle => 'Twinkle';

  @override
  String get errorBluetoothOff =>
      'Please turn on Bluetooth to connect to your PixelTree';

  @override
  String get errorLocationPermission =>
      'Location permission is required for Bluetooth scanning';

  @override
  String get errorBluetoothPermission =>
      'Bluetooth permission is required to connect';

  @override
  String get errorDeviceNotFound =>
      'Can\'t find your PixelTree. Make sure it\'s powered on and nearby.';

  @override
  String get errorConnectionFailed => 'Couldn\'t connect. Let\'s try again!';

  @override
  String get errorUnknown => 'Something went wrong. Please try again.';

  @override
  String get categoryStatic => 'Static';

  @override
  String get categoryWave => 'Wave';

  @override
  String get categoryChase => 'Chase';

  @override
  String get categoryTwinkle => 'Twinkle';

  @override
  String get categoryFire => 'Fire';

  @override
  String get categoryChristmas => 'Christmas';

  @override
  String get categorySpecial => 'Special';

  @override
  String get categoryBreathing => 'Breathing';

  @override
  String get categoryAlarm => 'Alarm';

  @override
  String get colorModeWarm => 'Warm';

  @override
  String get colorModeCold => 'Cold';

  @override
  String get colorModeMulti => 'Multi';

  @override
  String get colorModePalette => 'Palette';

  @override
  String get directionRight => 'Right →';

  @override
  String get directionLeft => '← Left';

  @override
  String get paramSpeed => 'Speed';

  @override
  String get paramColor => 'Color';

  @override
  String get paramBrightness => 'Brightness';

  @override
  String get paramIntensity => 'Intensity';

  @override
  String get paramSize => 'Size';

  @override
  String get paramWidth => 'Width';

  @override
  String get paramTrail => 'Trail';

  @override
  String get paramFrequency => 'Frequency';

  @override
  String get paramAmplitude => 'Amplitude';

  @override
  String get paramPalette => 'Palette';

  @override
  String get paramDirection => 'Direction';

  @override
  String get paramStyle => 'Style';

  @override
  String get paramMode => 'Mode';

  @override
  String get paramDual => 'Dual';

  @override
  String get paramRainbow => 'Rainbow';

  @override
  String get paramSparkle => 'Sparkle';

  @override
  String get paramFade => 'Fade';

  @override
  String get paramGap => 'Gap';

  @override
  String get paramDots => 'Dots';

  @override
  String get paramColors => 'Colors';

  @override
  String get colorPickerTitle => 'Pick a Color';
}
