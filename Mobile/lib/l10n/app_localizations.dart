import 'dart:async';

import 'package:flutter/foundation.dart';
import 'package:flutter/widgets.dart';
import 'package:flutter_localizations/flutter_localizations.dart';
import 'package:intl/intl.dart' as intl;

import 'app_localizations_en.dart';
import 'app_localizations_pl.dart';

// ignore_for_file: type=lint

/// Callers can lookup localized strings with an instance of AppLocalizations
/// returned by `AppLocalizations.of(context)`.
///
/// Applications need to include `AppLocalizations.delegate()` in their app's
/// `localizationDelegates` list, and the locales they support in the app's
/// `supportedLocales` list. For example:
///
/// ```dart
/// import 'l10n/app_localizations.dart';
///
/// return MaterialApp(
///   localizationsDelegates: AppLocalizations.localizationsDelegates,
///   supportedLocales: AppLocalizations.supportedLocales,
///   home: MyApplicationHome(),
/// );
/// ```
///
/// ## Update pubspec.yaml
///
/// Please make sure to update your pubspec.yaml to include the following
/// packages:
///
/// ```yaml
/// dependencies:
///   # Internationalization support.
///   flutter_localizations:
///     sdk: flutter
///   intl: any # Use the pinned version from flutter_localizations
///
///   # Rest of dependencies
/// ```
///
/// ## iOS Applications
///
/// iOS applications define key application metadata, including supported
/// locales, in an Info.plist file that is built into the application bundle.
/// To configure the locales supported by your app, you’ll need to edit this
/// file.
///
/// First, open your project’s ios/Runner.xcworkspace Xcode workspace file.
/// Then, in the Project Navigator, open the Info.plist file under the Runner
/// project’s Runner folder.
///
/// Next, select the Information Property List item, select Add Item from the
/// Editor menu, then select Localizations from the pop-up menu.
///
/// Select and expand the newly-created Localizations item then, for each
/// locale your application supports, add a new item and select the locale
/// you wish to add from the pop-up menu in the Value field. This list should
/// be consistent with the languages listed in the AppLocalizations.supportedLocales
/// property.
abstract class AppLocalizations {
  AppLocalizations(String locale)
    : localeName = intl.Intl.canonicalizedLocale(locale.toString());

  final String localeName;

  static AppLocalizations? of(BuildContext context) {
    return Localizations.of<AppLocalizations>(context, AppLocalizations);
  }

  static const LocalizationsDelegate<AppLocalizations> delegate =
      _AppLocalizationsDelegate();

  /// A list of this localizations delegate along with the default localizations
  /// delegates.
  ///
  /// Returns a list of localizations delegates containing this delegate along with
  /// GlobalMaterialLocalizations.delegate, GlobalCupertinoLocalizations.delegate,
  /// and GlobalWidgetsLocalizations.delegate.
  ///
  /// Additional delegates can be added by appending to this list in
  /// MaterialApp. This list does not have to be used at all if a custom list
  /// of delegates is preferred or required.
  static const List<LocalizationsDelegate<dynamic>> localizationsDelegates =
      <LocalizationsDelegate<dynamic>>[
        delegate,
        GlobalMaterialLocalizations.delegate,
        GlobalCupertinoLocalizations.delegate,
        GlobalWidgetsLocalizations.delegate,
      ];

  /// A list of this localizations delegate's supported locales.
  static const List<Locale> supportedLocales = <Locale>[
    Locale('en'),
    Locale('pl'),
  ];

  /// No description provided for @appName.
  ///
  /// In en, this message translates to:
  /// **'PixelTree'**
  String get appName;

  /// No description provided for @appTagline.
  ///
  /// In en, this message translates to:
  /// **'Smart ARGB Christmas Lights'**
  String get appTagline;

  /// No description provided for @onboardingWelcomeTitle.
  ///
  /// In en, this message translates to:
  /// **'Welcome to PixelTree! 🎄'**
  String get onboardingWelcomeTitle;

  /// No description provided for @onboardingWelcomeBody.
  ///
  /// In en, this message translates to:
  /// **'Transform your Christmas with smart ARGB lights'**
  String get onboardingWelcomeBody;

  /// No description provided for @onboardingSetupTitle.
  ///
  /// In en, this message translates to:
  /// **'Simple WiFi Setup'**
  String get onboardingSetupTitle;

  /// No description provided for @onboardingSetupBody.
  ///
  /// In en, this message translates to:
  /// **'Connect your lights to WiFi in just a few taps'**
  String get onboardingSetupBody;

  /// No description provided for @onboardingControlTitle.
  ///
  /// In en, this message translates to:
  /// **'Endless Possibilities'**
  String get onboardingControlTitle;

  /// No description provided for @onboardingControlBody.
  ///
  /// In en, this message translates to:
  /// **'Choose colors, effects, and create magic'**
  String get onboardingControlBody;

  /// No description provided for @onboardingGetStarted.
  ///
  /// In en, this message translates to:
  /// **'Let\'s Get Started!'**
  String get onboardingGetStarted;

  /// No description provided for @onboardingDontShowAgain.
  ///
  /// In en, this message translates to:
  /// **'Don\'t show this again'**
  String get onboardingDontShowAgain;

  /// No description provided for @connectionModeTitle.
  ///
  /// In en, this message translates to:
  /// **'WiFi Setup Method'**
  String get connectionModeTitle;

  /// No description provided for @connectionModeSubtitle.
  ///
  /// In en, this message translates to:
  /// **'Choose how to configure your PixelTree'**
  String get connectionModeSubtitle;

  /// No description provided for @connectionRecommended.
  ///
  /// In en, this message translates to:
  /// **'RECOMMENDED'**
  String get connectionRecommended;

  /// No description provided for @connectionBleWifiTitle.
  ///
  /// In en, this message translates to:
  /// **'BLE + WiFi'**
  String get connectionBleWifiTitle;

  /// No description provided for @connectionBleWifiSubtitle.
  ///
  /// In en, this message translates to:
  /// **'Bluetooth Low Energy'**
  String get connectionBleWifiSubtitle;

  /// No description provided for @connectionBleWifiDescription.
  ///
  /// In en, this message translates to:
  /// **'Connect via Bluetooth and configure WiFi settings. Your PixelTree will then join your WiFi network.'**
  String get connectionBleWifiDescription;

  /// No description provided for @connectionBleWifiButton.
  ///
  /// In en, this message translates to:
  /// **'Start Setup'**
  String get connectionBleWifiButton;

  /// No description provided for @connectionApTitle.
  ///
  /// In en, this message translates to:
  /// **'Access Point Mode'**
  String get connectionApTitle;

  /// No description provided for @connectionApDescription.
  ///
  /// In en, this message translates to:
  /// **'Direct WiFi connection to PixelTree'**
  String get connectionApDescription;

  /// No description provided for @connectionModeHelp.
  ///
  /// In en, this message translates to:
  /// **'Not sure? Use the recommended method for easiest setup.'**
  String get connectionModeHelp;

  /// No description provided for @connectionAlreadyHaveDevices.
  ///
  /// In en, this message translates to:
  /// **'Already have devices set up?'**
  String get connectionAlreadyHaveDevices;

  /// No description provided for @apConnectionTitle.
  ///
  /// In en, this message translates to:
  /// **'Connect to Access Point'**
  String get apConnectionTitle;

  /// No description provided for @apConnectionSubtitle.
  ///
  /// In en, this message translates to:
  /// **'Follow these steps to connect'**
  String get apConnectionSubtitle;

  /// No description provided for @apConnectionStep1Title.
  ///
  /// In en, this message translates to:
  /// **'Open WiFi Settings'**
  String get apConnectionStep1Title;

  /// No description provided for @apConnectionStep1Desc.
  ///
  /// In en, this message translates to:
  /// **'Go to your phone\'s WiFi settings'**
  String get apConnectionStep1Desc;

  /// No description provided for @apConnectionStep2Title.
  ///
  /// In en, this message translates to:
  /// **'Find PixelTree Network'**
  String get apConnectionStep2Title;

  /// No description provided for @apConnectionStep2Desc.
  ///
  /// In en, this message translates to:
  /// **'Look for \"PixelTree-XXXX\"'**
  String get apConnectionStep2Desc;

  /// No description provided for @apConnectionStep3Title.
  ///
  /// In en, this message translates to:
  /// **'Connect'**
  String get apConnectionStep3Title;

  /// No description provided for @apConnectionStep3Desc.
  ///
  /// In en, this message translates to:
  /// **'Tap to connect (no password)'**
  String get apConnectionStep3Desc;

  /// No description provided for @apConnectionStep4Title.
  ///
  /// In en, this message translates to:
  /// **'Return Here'**
  String get apConnectionStep4Title;

  /// No description provided for @apConnectionStep4Desc.
  ///
  /// In en, this message translates to:
  /// **'Come back and tap \"Continue\"'**
  String get apConnectionStep4Desc;

  /// No description provided for @apConnectionInfo.
  ///
  /// In en, this message translates to:
  /// **'Phone will temporarily disconnect from internet'**
  String get apConnectionInfo;

  /// No description provided for @apConnectionButton.
  ///
  /// In en, this message translates to:
  /// **'I\'m Connected'**
  String get apConnectionButton;

  /// No description provided for @apConnectionNotConnected.
  ///
  /// In en, this message translates to:
  /// **'Not Connected'**
  String get apConnectionNotConnected;

  /// No description provided for @apConnectionNotConnectedMessage.
  ///
  /// In en, this message translates to:
  /// **'You are connected to {wifiName}.\n\nPlease connect to the PixelTree access point first.'**
  String apConnectionNotConnectedMessage(String wifiName);

  /// No description provided for @apConnectionNotConnectedNoWifi.
  ///
  /// In en, this message translates to:
  /// **'Please connect to the PixelTree access point first.'**
  String get apConnectionNotConnectedNoWifi;

  /// No description provided for @apConnectionPermissionRequired.
  ///
  /// In en, this message translates to:
  /// **'Permission Required'**
  String get apConnectionPermissionRequired;

  /// No description provided for @apConnectionPermissionMessage.
  ///
  /// In en, this message translates to:
  /// **'Location permission is required to detect WiFi network.\n\nPlease grant permission in Settings.'**
  String get apConnectionPermissionMessage;

  /// No description provided for @apConnectionOpenSettings.
  ///
  /// In en, this message translates to:
  /// **'Open Settings'**
  String get apConnectionOpenSettings;

  /// No description provided for @scanTitle.
  ///
  /// In en, this message translates to:
  /// **'Find Your PixelTree'**
  String get scanTitle;

  /// No description provided for @scanScanning.
  ///
  /// In en, this message translates to:
  /// **'Searching for devices...'**
  String get scanScanning;

  /// No description provided for @scanNoDevices.
  ///
  /// In en, this message translates to:
  /// **'No devices found. Make sure your PixelTree is powered on.'**
  String get scanNoDevices;

  /// No description provided for @scanRefresh.
  ///
  /// In en, this message translates to:
  /// **'Refresh Scan'**
  String get scanRefresh;

  /// No description provided for @scanConnect.
  ///
  /// In en, this message translates to:
  /// **'Connect'**
  String get scanConnect;

  /// No description provided for @scanConfigured.
  ///
  /// In en, this message translates to:
  /// **'Configured'**
  String get scanConfigured;

  /// No description provided for @scanReadyToConnect.
  ///
  /// In en, this message translates to:
  /// **'Ready to connect'**
  String get scanReadyToConnect;

  /// No description provided for @provisioningTitle.
  ///
  /// In en, this message translates to:
  /// **'WiFi Setup Wizard'**
  String get provisioningTitle;

  /// No description provided for @provisioningStep.
  ///
  /// In en, this message translates to:
  /// **'Step'**
  String get provisioningStep;

  /// No description provided for @provisioningOf.
  ///
  /// In en, this message translates to:
  /// **'of'**
  String get provisioningOf;

  /// No description provided for @provisioningKeyExchangeTitle.
  ///
  /// In en, this message translates to:
  /// **'Establishing Secure Connection...'**
  String get provisioningKeyExchangeTitle;

  /// No description provided for @provisioningKeyExchangeBody.
  ///
  /// In en, this message translates to:
  /// **'Exchanging encryption keys...\nThis keeps your WiFi password safe during transfer.'**
  String get provisioningKeyExchangeBody;

  /// No description provided for @provisioningSelectNetworkTitle.
  ///
  /// In en, this message translates to:
  /// **'Select Your WiFi Network'**
  String get provisioningSelectNetworkTitle;

  /// No description provided for @provisioningScanningNetworks.
  ///
  /// In en, this message translates to:
  /// **'Scanning WiFi Networks'**
  String get provisioningScanningNetworks;

  /// No description provided for @provisioningScanningMessage.
  ///
  /// In en, this message translates to:
  /// **'Please wait while your device scans for available networks...'**
  String get provisioningScanningMessage;

  /// No description provided for @provisioningEnterPasswordTitle.
  ///
  /// In en, this message translates to:
  /// **'Enter WiFi Password'**
  String get provisioningEnterPasswordTitle;

  /// No description provided for @provisioningPasswordHint.
  ///
  /// In en, this message translates to:
  /// **'Password...'**
  String get provisioningPasswordHint;

  /// No description provided for @provisioningPasswordInfo.
  ///
  /// In en, this message translates to:
  /// **'Your password is encrypted before being sent to the device using AES-128.'**
  String get provisioningPasswordInfo;

  /// No description provided for @provisioningNetworkLabel.
  ///
  /// In en, this message translates to:
  /// **'Network: {ssid}'**
  String provisioningNetworkLabel(String ssid);

  /// No description provided for @provisioningConnectingTitle.
  ///
  /// In en, this message translates to:
  /// **'Connecting to WiFi...'**
  String get provisioningConnectingTitle;

  /// No description provided for @provisioningConnectingBody.
  ///
  /// In en, this message translates to:
  /// **'PixelTree is connecting to your network.\n\nThis may take up to 30 seconds...'**
  String get provisioningConnectingBody;

  /// No description provided for @provisioningSuccessTitle.
  ///
  /// In en, this message translates to:
  /// **'Connected Successfully!'**
  String get provisioningSuccessTitle;

  /// No description provided for @provisioningFailedTitle.
  ///
  /// In en, this message translates to:
  /// **'Connection Failed'**
  String get provisioningFailedTitle;

  /// No description provided for @provisioningFailedBody.
  ///
  /// In en, this message translates to:
  /// **'Could not connect to WiFi. Please check your password and try again.'**
  String get provisioningFailedBody;

  /// No description provided for @provisioningConnectionFailed.
  ///
  /// In en, this message translates to:
  /// **'Connection failed: {error}'**
  String provisioningConnectionFailed(String error);

  /// No description provided for @provisioningWrongPassword.
  ///
  /// In en, this message translates to:
  /// **'Wrong WiFi password. Please check and try again.'**
  String get provisioningWrongPassword;

  /// No description provided for @provisioningSsidNotFound.
  ///
  /// In en, this message translates to:
  /// **'WiFi network disappeared. Please select another.'**
  String get provisioningSsidNotFound;

  /// No description provided for @provisioningAuthFailed.
  ///
  /// In en, this message translates to:
  /// **'WiFi authentication failed. Please check password.'**
  String get provisioningAuthFailed;

  /// No description provided for @provisioningTimeout.
  ///
  /// In en, this message translates to:
  /// **'Connection timeout. Please try again.'**
  String get provisioningTimeout;

  /// No description provided for @provisioningError.
  ///
  /// In en, this message translates to:
  /// **'Error: {error}'**
  String provisioningError(String error);

  /// No description provided for @buttonContinue.
  ///
  /// In en, this message translates to:
  /// **'Continue'**
  String get buttonContinue;

  /// No description provided for @buttonBack.
  ///
  /// In en, this message translates to:
  /// **'Back'**
  String get buttonBack;

  /// No description provided for @buttonRetry.
  ///
  /// In en, this message translates to:
  /// **'Retry'**
  String get buttonRetry;

  /// No description provided for @buttonSkip.
  ///
  /// In en, this message translates to:
  /// **'Skip'**
  String get buttonSkip;

  /// No description provided for @buttonConnect.
  ///
  /// In en, this message translates to:
  /// **'Connect'**
  String get buttonConnect;

  /// No description provided for @buttonCancel.
  ///
  /// In en, this message translates to:
  /// **'Cancel'**
  String get buttonCancel;

  /// No description provided for @buttonOk.
  ///
  /// In en, this message translates to:
  /// **'OK'**
  String get buttonOk;

  /// No description provided for @buttonRefresh.
  ///
  /// In en, this message translates to:
  /// **'Refresh'**
  String get buttonRefresh;

  /// No description provided for @myDevicesTitle.
  ///
  /// In en, this message translates to:
  /// **'My Devices'**
  String get myDevicesTitle;

  /// No description provided for @myDevicesAddDevice.
  ///
  /// In en, this message translates to:
  /// **'Add Device'**
  String get myDevicesAddDevice;

  /// No description provided for @myDevicesSavedDevices.
  ///
  /// In en, this message translates to:
  /// **'Saved Devices'**
  String get myDevicesSavedDevices;

  /// No description provided for @myDevicesDiscovered.
  ///
  /// In en, this message translates to:
  /// **'Discovered on Network'**
  String get myDevicesDiscovered;

  /// No description provided for @myDevicesNoSaved.
  ///
  /// In en, this message translates to:
  /// **'No saved devices yet'**
  String get myDevicesNoSaved;

  /// No description provided for @myDevicesNoSavedSubtitle.
  ///
  /// In en, this message translates to:
  /// **'Add devices via BLE or save discovered ones below'**
  String get myDevicesNoSavedSubtitle;

  /// No description provided for @myDevicesNoDiscovered.
  ///
  /// In en, this message translates to:
  /// **'No new devices found'**
  String get myDevicesNoDiscovered;

  /// No description provided for @myDevicesNoDiscoveredSubtitle.
  ///
  /// In en, this message translates to:
  /// **'Make sure devices are powered on and connected to the same WiFi'**
  String get myDevicesNoDiscoveredSubtitle;

  /// No description provided for @myDevicesScanning.
  ///
  /// In en, this message translates to:
  /// **'Scanning network...'**
  String get myDevicesScanning;

  /// No description provided for @myDevicesNoDevicesYet.
  ///
  /// In en, this message translates to:
  /// **'No Devices Yet'**
  String get myDevicesNoDevicesYet;

  /// No description provided for @myDevicesNoDevicesSubtitle.
  ///
  /// In en, this message translates to:
  /// **'Add your first PixelTree device\nto get started'**
  String get myDevicesNoDevicesSubtitle;

  /// No description provided for @myDevicesRemoveDevice.
  ///
  /// In en, this message translates to:
  /// **'Remove Device'**
  String get myDevicesRemoveDevice;

  /// No description provided for @myDevicesRemoveTitle.
  ///
  /// In en, this message translates to:
  /// **'Remove Device?'**
  String get myDevicesRemoveTitle;

  /// No description provided for @myDevicesRemoveMessage.
  ///
  /// In en, this message translates to:
  /// **'Remove {deviceName} from your saved devices?'**
  String myDevicesRemoveMessage(String deviceName);

  /// No description provided for @myDevicesRemoved.
  ///
  /// In en, this message translates to:
  /// **'{deviceName} removed'**
  String myDevicesRemoved(String deviceName);

  /// No description provided for @myDevicesSaved.
  ///
  /// In en, this message translates to:
  /// **'{deviceName} saved!'**
  String myDevicesSaved(String deviceName);

  /// No description provided for @myDevicesOfflineTitle.
  ///
  /// In en, this message translates to:
  /// **'{deviceName} Offline'**
  String myDevicesOfflineTitle(String deviceName);

  /// No description provided for @myDevicesOfflineMessage.
  ///
  /// In en, this message translates to:
  /// **'Device is not reachable on the network.'**
  String get myDevicesOfflineMessage;

  /// No description provided for @myDevicesTroubleshooting.
  ///
  /// In en, this message translates to:
  /// **'Troubleshooting tips:'**
  String get myDevicesTroubleshooting;

  /// No description provided for @myDevicesTip1.
  ///
  /// In en, this message translates to:
  /// **'• Check device is powered on'**
  String get myDevicesTip1;

  /// No description provided for @myDevicesTip2.
  ///
  /// In en, this message translates to:
  /// **'• Verify device is connected to WiFi'**
  String get myDevicesTip2;

  /// No description provided for @myDevicesTip3.
  ///
  /// In en, this message translates to:
  /// **'• Ensure phone is on same network'**
  String get myDevicesTip3;

  /// No description provided for @myDevicesTip4.
  ///
  /// In en, this message translates to:
  /// **'• Try refreshing device status'**
  String get myDevicesTip4;

  /// No description provided for @myDevicesSearching.
  ///
  /// In en, this message translates to:
  /// **'Searching for device...'**
  String get myDevicesSearching;

  /// No description provided for @myDevicesNotFound.
  ///
  /// In en, this message translates to:
  /// **'Device not found on network.\n\nMake sure:\n• Device is powered on\n• Device is connected to WiFi\n• Phone is on same network'**
  String get myDevicesNotFound;

  /// No description provided for @myDevicesOnline.
  ///
  /// In en, this message translates to:
  /// **'Online'**
  String get myDevicesOnline;

  /// No description provided for @myDevicesOffline.
  ///
  /// In en, this message translates to:
  /// **'Offline'**
  String get myDevicesOffline;

  /// No description provided for @myDevicesScanningBtn.
  ///
  /// In en, this message translates to:
  /// **'Scanning...'**
  String get myDevicesScanningBtn;

  /// No description provided for @myDevicesCheckingBtn.
  ///
  /// In en, this message translates to:
  /// **'Checking...'**
  String get myDevicesCheckingBtn;

  /// No description provided for @myDevicesRefreshBtn.
  ///
  /// In en, this message translates to:
  /// **'Refresh'**
  String get myDevicesRefreshBtn;

  /// No description provided for @myDevicesDelete.
  ///
  /// In en, this message translates to:
  /// **'Delete'**
  String get myDevicesDelete;

  /// No description provided for @myDevicesNew.
  ///
  /// In en, this message translates to:
  /// **'NEW'**
  String get myDevicesNew;

  /// No description provided for @myDevicesDeviceOffline.
  ///
  /// In en, this message translates to:
  /// **'{deviceName} Offline'**
  String myDevicesDeviceOffline(String deviceName);

  /// No description provided for @myDevicesLastSeenJustNow.
  ///
  /// In en, this message translates to:
  /// **'Last seen just now'**
  String get myDevicesLastSeenJustNow;

  /// No description provided for @myDevicesLastSeenMinutes.
  ///
  /// In en, this message translates to:
  /// **'Last seen {minutes} min ago'**
  String myDevicesLastSeenMinutes(int minutes);

  /// No description provided for @myDevicesLastSeenHours.
  ///
  /// In en, this message translates to:
  /// **'Last seen {hours} hours ago'**
  String myDevicesLastSeenHours(int hours);

  /// No description provided for @myDevicesLastSeenDays.
  ///
  /// In en, this message translates to:
  /// **'Last seen {days} days ago'**
  String myDevicesLastSeenDays(int days);

  /// No description provided for @myDevicesLastSeenWeeks.
  ///
  /// In en, this message translates to:
  /// **'Last seen {weeks} weeks ago'**
  String myDevicesLastSeenWeeks(int weeks);

  /// No description provided for @ledControlNoConnection.
  ///
  /// In en, this message translates to:
  /// **'No Connection'**
  String get ledControlNoConnection;

  /// No description provided for @ledControlTryAgain.
  ///
  /// In en, this message translates to:
  /// **'Try Again'**
  String get ledControlTryAgain;

  /// No description provided for @dashboardTitle.
  ///
  /// In en, this message translates to:
  /// **'LED Control'**
  String get dashboardTitle;

  /// No description provided for @dashboardConnected.
  ///
  /// In en, this message translates to:
  /// **'Connected'**
  String get dashboardConnected;

  /// No description provided for @dashboardDisconnected.
  ///
  /// In en, this message translates to:
  /// **'Disconnected'**
  String get dashboardDisconnected;

  /// No description provided for @dashboardColor.
  ///
  /// In en, this message translates to:
  /// **'Color'**
  String get dashboardColor;

  /// No description provided for @dashboardBrightness.
  ///
  /// In en, this message translates to:
  /// **'Brightness'**
  String get dashboardBrightness;

  /// No description provided for @dashboardEffects.
  ///
  /// In en, this message translates to:
  /// **'Effects'**
  String get dashboardEffects;

  /// No description provided for @dashboardApply.
  ///
  /// In en, this message translates to:
  /// **'Apply Changes'**
  String get dashboardApply;

  /// No description provided for @effectSolid.
  ///
  /// In en, this message translates to:
  /// **'Solid'**
  String get effectSolid;

  /// No description provided for @effectFade.
  ///
  /// In en, this message translates to:
  /// **'Fade'**
  String get effectFade;

  /// No description provided for @effectRainbow.
  ///
  /// In en, this message translates to:
  /// **'Rainbow'**
  String get effectRainbow;

  /// No description provided for @effectChase.
  ///
  /// In en, this message translates to:
  /// **'Chase'**
  String get effectChase;

  /// No description provided for @effectTwinkle.
  ///
  /// In en, this message translates to:
  /// **'Twinkle'**
  String get effectTwinkle;

  /// No description provided for @errorBluetoothOff.
  ///
  /// In en, this message translates to:
  /// **'Please turn on Bluetooth to connect to your PixelTree'**
  String get errorBluetoothOff;

  /// No description provided for @errorLocationPermission.
  ///
  /// In en, this message translates to:
  /// **'Location permission is required for Bluetooth scanning'**
  String get errorLocationPermission;

  /// No description provided for @errorBluetoothPermission.
  ///
  /// In en, this message translates to:
  /// **'Bluetooth permission is required to connect'**
  String get errorBluetoothPermission;

  /// No description provided for @errorDeviceNotFound.
  ///
  /// In en, this message translates to:
  /// **'Can\'t find your PixelTree. Make sure it\'s powered on and nearby.'**
  String get errorDeviceNotFound;

  /// No description provided for @errorConnectionFailed.
  ///
  /// In en, this message translates to:
  /// **'Couldn\'t connect. Let\'s try again!'**
  String get errorConnectionFailed;

  /// No description provided for @errorUnknown.
  ///
  /// In en, this message translates to:
  /// **'Something went wrong. Please try again.'**
  String get errorUnknown;

  /// No description provided for @categoryStatic.
  ///
  /// In en, this message translates to:
  /// **'Static'**
  String get categoryStatic;

  /// No description provided for @categoryWave.
  ///
  /// In en, this message translates to:
  /// **'Wave'**
  String get categoryWave;

  /// No description provided for @categoryChase.
  ///
  /// In en, this message translates to:
  /// **'Chase'**
  String get categoryChase;

  /// No description provided for @categoryTwinkle.
  ///
  /// In en, this message translates to:
  /// **'Twinkle'**
  String get categoryTwinkle;

  /// No description provided for @categoryFire.
  ///
  /// In en, this message translates to:
  /// **'Fire'**
  String get categoryFire;

  /// No description provided for @categoryChristmas.
  ///
  /// In en, this message translates to:
  /// **'Christmas'**
  String get categoryChristmas;

  /// No description provided for @categorySpecial.
  ///
  /// In en, this message translates to:
  /// **'Special'**
  String get categorySpecial;

  /// No description provided for @categoryBreathing.
  ///
  /// In en, this message translates to:
  /// **'Breathing'**
  String get categoryBreathing;

  /// No description provided for @categoryAlarm.
  ///
  /// In en, this message translates to:
  /// **'Alarm'**
  String get categoryAlarm;

  /// No description provided for @colorModeWarm.
  ///
  /// In en, this message translates to:
  /// **'Warm'**
  String get colorModeWarm;

  /// No description provided for @colorModeCold.
  ///
  /// In en, this message translates to:
  /// **'Cold'**
  String get colorModeCold;

  /// No description provided for @colorModeMulti.
  ///
  /// In en, this message translates to:
  /// **'Multi'**
  String get colorModeMulti;

  /// No description provided for @colorModePalette.
  ///
  /// In en, this message translates to:
  /// **'Palette'**
  String get colorModePalette;

  /// No description provided for @directionRight.
  ///
  /// In en, this message translates to:
  /// **'Right →'**
  String get directionRight;

  /// No description provided for @directionLeft.
  ///
  /// In en, this message translates to:
  /// **'← Left'**
  String get directionLeft;

  /// No description provided for @paramSpeed.
  ///
  /// In en, this message translates to:
  /// **'Speed'**
  String get paramSpeed;

  /// No description provided for @paramColor.
  ///
  /// In en, this message translates to:
  /// **'Color'**
  String get paramColor;

  /// No description provided for @paramBrightness.
  ///
  /// In en, this message translates to:
  /// **'Brightness'**
  String get paramBrightness;

  /// No description provided for @paramIntensity.
  ///
  /// In en, this message translates to:
  /// **'Intensity'**
  String get paramIntensity;

  /// No description provided for @paramSize.
  ///
  /// In en, this message translates to:
  /// **'Size'**
  String get paramSize;

  /// No description provided for @paramWidth.
  ///
  /// In en, this message translates to:
  /// **'Width'**
  String get paramWidth;

  /// No description provided for @paramTrail.
  ///
  /// In en, this message translates to:
  /// **'Trail'**
  String get paramTrail;

  /// No description provided for @paramFrequency.
  ///
  /// In en, this message translates to:
  /// **'Frequency'**
  String get paramFrequency;

  /// No description provided for @paramAmplitude.
  ///
  /// In en, this message translates to:
  /// **'Amplitude'**
  String get paramAmplitude;

  /// No description provided for @paramPalette.
  ///
  /// In en, this message translates to:
  /// **'Palette'**
  String get paramPalette;

  /// No description provided for @paramDirection.
  ///
  /// In en, this message translates to:
  /// **'Direction'**
  String get paramDirection;

  /// No description provided for @paramStyle.
  ///
  /// In en, this message translates to:
  /// **'Style'**
  String get paramStyle;

  /// No description provided for @paramMode.
  ///
  /// In en, this message translates to:
  /// **'Mode'**
  String get paramMode;

  /// No description provided for @paramDual.
  ///
  /// In en, this message translates to:
  /// **'Dual'**
  String get paramDual;

  /// No description provided for @paramRainbow.
  ///
  /// In en, this message translates to:
  /// **'Rainbow'**
  String get paramRainbow;

  /// No description provided for @paramSparkle.
  ///
  /// In en, this message translates to:
  /// **'Sparkle'**
  String get paramSparkle;

  /// No description provided for @paramFade.
  ///
  /// In en, this message translates to:
  /// **'Fade'**
  String get paramFade;

  /// No description provided for @paramGap.
  ///
  /// In en, this message translates to:
  /// **'Gap'**
  String get paramGap;

  /// No description provided for @paramDots.
  ///
  /// In en, this message translates to:
  /// **'Dots'**
  String get paramDots;

  /// No description provided for @paramColors.
  ///
  /// In en, this message translates to:
  /// **'Colors'**
  String get paramColors;

  /// No description provided for @colorPickerTitle.
  ///
  /// In en, this message translates to:
  /// **'Pick a Color'**
  String get colorPickerTitle;
}

class _AppLocalizationsDelegate
    extends LocalizationsDelegate<AppLocalizations> {
  const _AppLocalizationsDelegate();

  @override
  Future<AppLocalizations> load(Locale locale) {
    return SynchronousFuture<AppLocalizations>(lookupAppLocalizations(locale));
  }

  @override
  bool isSupported(Locale locale) =>
      <String>['en', 'pl'].contains(locale.languageCode);

  @override
  bool shouldReload(_AppLocalizationsDelegate old) => false;
}

AppLocalizations lookupAppLocalizations(Locale locale) {
  // Lookup logic when only language code is specified.
  switch (locale.languageCode) {
    case 'en':
      return AppLocalizationsEn();
    case 'pl':
      return AppLocalizationsPl();
  }

  throw FlutterError(
    'AppLocalizations.delegate failed to load unsupported locale "$locale". This is likely '
    'an issue with the localizations generation tool. Please file an issue '
    'on GitHub with a reproducible sample app and the gen-l10n configuration '
    'that was used.',
  );
}
