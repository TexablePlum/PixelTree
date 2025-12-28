// ignore: unused_import
import 'package:intl/intl.dart' as intl;
import 'app_localizations.dart';

// ignore_for_file: type=lint

/// The translations for Polish (`pl`).
class AppLocalizationsPl extends AppLocalizations {
  AppLocalizationsPl([String locale = 'pl']) : super(locale);

  @override
  String get appName => 'PixelTree';

  @override
  String get appTagline => 'Inteligentne lampki choinkowe ARGB';

  @override
  String get onboardingWelcomeTitle => 'Witaj w PixelTree! 🎄';

  @override
  String get onboardingWelcomeBody =>
      'Odmień swoje święta dzięki inteligentnym lampkom ARGB';

  @override
  String get onboardingSetupTitle => 'Prosta konfiguracja WiFi';

  @override
  String get onboardingSetupBody =>
      'Połącz lampki z WiFi w kilku prostych krokach';

  @override
  String get onboardingControlTitle => 'Nieskończone możliwości';

  @override
  String get onboardingControlBody => 'Wybieraj kolory, efekty i twórz magię';

  @override
  String get onboardingGetStarted => 'Zaczynamy!';

  @override
  String get onboardingDontShowAgain => 'Nie pokazuj ponownie';

  @override
  String get connectionModeTitle => 'Metoda konfiguracji WiFi';

  @override
  String get connectionModeSubtitle => 'Wybierz sposób konfiguracji PixelTree';

  @override
  String get connectionRecommended => 'ZALECANE';

  @override
  String get connectionBleWifiTitle => 'BLE + WiFi';

  @override
  String get connectionBleWifiSubtitle => 'Bluetooth Low Energy';

  @override
  String get connectionBleWifiDescription =>
      'Połącz przez Bluetooth i skonfiguruj ustawienia WiFi. PixelTree dołączy do Twojej sieci WiFi.';

  @override
  String get connectionBleWifiButton => 'Rozpocznij konfigurację';

  @override
  String get connectionApTitle => 'Tryb Access Point';

  @override
  String get connectionApDescription =>
      'Bezpośrednie połączenie WiFi z PixelTree';

  @override
  String get connectionModeHelp =>
      'Nie wiesz? Użyj zalecanej metody - to najprostsze.';

  @override
  String get connectionAlreadyHaveDevices =>
      'Masz już skonfigurowane urządzenia?';

  @override
  String get apConnectionTitle => 'Połącz z Access Pointem';

  @override
  String get apConnectionSubtitle => 'Wykonaj następujące kroki';

  @override
  String get apConnectionStep1Title => 'Otwórz ustawienia WiFi';

  @override
  String get apConnectionStep1Desc => 'Przejdź do ustawień WiFi telefonu';

  @override
  String get apConnectionStep2Title => 'Znajdź sieć PixelTree';

  @override
  String get apConnectionStep2Desc => 'Szukaj \"PixelTree-XXXX\"';

  @override
  String get apConnectionStep3Title => 'Połącz';

  @override
  String get apConnectionStep3Desc => 'Kliknij aby połączyć (bez hasła)';

  @override
  String get apConnectionStep4Title => 'Wróć tutaj';

  @override
  String get apConnectionStep4Desc => 'Wróć i kliknij \"Kontynuuj\"';

  @override
  String get apConnectionInfo =>
      'Telefon tymczasowo straci połączenie z internetem';

  @override
  String get apConnectionButton => 'Połączono';

  @override
  String get apConnectionNotConnected => 'Brak połączenia';

  @override
  String apConnectionNotConnectedMessage(String wifiName) {
    return 'Jesteś połączony z $wifiName.\n\nNajpierw połącz się z access pointem PixelTree.';
  }

  @override
  String get apConnectionNotConnectedNoWifi =>
      'Najpierw połącz się z access pointem PixelTree.';

  @override
  String get apConnectionPermissionRequired => 'Wymagane uprawnienie';

  @override
  String get apConnectionPermissionMessage =>
      'Uprawnienie do lokalizacji jest wymagane do wykrycia sieci WiFi.\n\nPrzyznaj uprawnienie w Ustawieniach.';

  @override
  String get apConnectionOpenSettings => 'Otwórz ustawienia';

  @override
  String get scanTitle => 'Znajdź swoje PixelTree';

  @override
  String get scanScanning => 'Szukanie urządzeń...';

  @override
  String get scanNoDevices =>
      'Nie znaleziono urządzeń. Upewnij się, że PixelTree jest włączone.';

  @override
  String get scanRefresh => 'Odśwież skanowanie';

  @override
  String get scanConnect => 'Połącz';

  @override
  String get scanConfigured => 'Skonfigurowane';

  @override
  String get scanReadyToConnect => 'Gotowe do połączenia';

  @override
  String get provisioningTitle => 'Kreator konfiguracji WiFi';

  @override
  String get provisioningStep => 'Krok';

  @override
  String get provisioningOf => 'z';

  @override
  String get provisioningKeyExchangeTitle =>
      'Nawiązywanie bezpiecznego połączenia...';

  @override
  String get provisioningKeyExchangeBody =>
      'Wymiana kluczy szyfrowania...\nTo chroni Twoje hasło WiFi podczas przesyłania.';

  @override
  String get provisioningSelectNetworkTitle => 'Wybierz swoją sieć WiFi';

  @override
  String get provisioningScanningNetworks => 'Skanowanie sieci WiFi';

  @override
  String get provisioningScanningMessage =>
      'Proszę czekać, urządzenie skanuje dostępne sieci...';

  @override
  String get provisioningEnterPasswordTitle => 'Wprowadź hasło WiFi';

  @override
  String get provisioningPasswordHint => 'Hasło...';

  @override
  String get provisioningPasswordInfo =>
      'Twoje hasło jest szyfrowane przed wysłaniem do urządzenia za pomocą AES-128.';

  @override
  String provisioningNetworkLabel(String ssid) {
    return 'Sieć: $ssid';
  }

  @override
  String get provisioningConnectingTitle => 'Łączenie z WiFi...';

  @override
  String get provisioningConnectingBody =>
      'PixelTree łączy się z Twoją siecią.\n\nTo może potrwać do 30 sekund...';

  @override
  String get provisioningSuccessTitle => 'Połączono pomyślnie!';

  @override
  String get provisioningFailedTitle => 'Połączenie nieudane';

  @override
  String get provisioningFailedBody =>
      'Nie udało się połączyć z WiFi. Sprawdź hasło i spróbuj ponownie.';

  @override
  String provisioningConnectionFailed(String error) {
    return 'Połączenie nieudane: $error';
  }

  @override
  String get provisioningWrongPassword =>
      'Błędne hasło WiFi. Sprawdź i spróbuj ponownie.';

  @override
  String get provisioningSsidNotFound => 'Sieć WiFi zniknęła. Wybierz inną.';

  @override
  String get provisioningAuthFailed =>
      'Uwierzytelnianie WiFi nieudane. Sprawdź hasło.';

  @override
  String get provisioningTimeout =>
      'Przekroczono czas oczekiwania. Spróbuj ponownie.';

  @override
  String provisioningError(String error) {
    return 'Błąd: $error';
  }

  @override
  String get buttonContinue => 'Kontynuuj';

  @override
  String get buttonBack => 'Wstecz';

  @override
  String get buttonRetry => 'Ponów';

  @override
  String get buttonSkip => 'Pomiń';

  @override
  String get buttonConnect => 'Połącz';

  @override
  String get buttonCancel => 'Anuluj';

  @override
  String get buttonOk => 'OK';

  @override
  String get buttonRefresh => 'Odśwież';

  @override
  String get myDevicesTitle => 'Moje urządzenia';

  @override
  String get myDevicesAddDevice => 'Dodaj urządzenie';

  @override
  String get myDevicesSavedDevices => 'Zapisane urządzenia';

  @override
  String get myDevicesDiscovered => 'Znalezione w sieci';

  @override
  String get myDevicesNoSaved => 'Brak zapisanych urządzeń';

  @override
  String get myDevicesNoSavedSubtitle =>
      'Dodaj urządzenia przez BLE lub zapisz znalezione poniżej';

  @override
  String get myDevicesNoDiscovered => 'Nie znaleziono nowych urządzeń';

  @override
  String get myDevicesNoDiscoveredSubtitle =>
      'Upewnij się, że urządzenia są włączone i połączone z tym samym WiFi';

  @override
  String get myDevicesScanning => 'Skanowanie sieci...';

  @override
  String get myDevicesNoDevicesYet => 'Brak urządzeń';

  @override
  String get myDevicesNoDevicesSubtitle =>
      'Dodaj swoje pierwsze urządzenie PixelTree\naby rozpocząć';

  @override
  String get myDevicesRemoveDevice => 'Usuń urządzenie';

  @override
  String get myDevicesRemoveTitle => 'Usunąć urządzenie?';

  @override
  String myDevicesRemoveMessage(String deviceName) {
    return 'Usunąć $deviceName z zapisanych urządzeń?';
  }

  @override
  String myDevicesRemoved(String deviceName) {
    return '$deviceName usunięte';
  }

  @override
  String myDevicesSaved(String deviceName) {
    return '$deviceName zapisane!';
  }

  @override
  String myDevicesOfflineTitle(String deviceName) {
    return '$deviceName offline';
  }

  @override
  String get myDevicesOfflineMessage => 'Urządzenie jest nieosiągalne w sieci.';

  @override
  String get myDevicesTroubleshooting => 'Wskazówki:';

  @override
  String get myDevicesTip1 => '• Sprawdź czy urządzenie jest włączone';

  @override
  String get myDevicesTip2 => '• Sprawdź czy urządzenie jest połączone z WiFi';

  @override
  String get myDevicesTip3 =>
      '• Upewnij się, że telefon jest w tej samej sieci';

  @override
  String get myDevicesTip4 => '• Spróbuj odświeżyć status urządzenia';

  @override
  String get myDevicesSearching => 'Szukanie urządzenia...';

  @override
  String get myDevicesNotFound =>
      'Nie znaleziono urządzenia w sieci.\n\nUpewnij się, że:\n• Urządzenie jest włączone\n• Urządzenie jest połączone z WiFi\n• Telefon jest w tej samej sieci';

  @override
  String get myDevicesOnline => 'Online';

  @override
  String get myDevicesOffline => 'Offline';

  @override
  String get myDevicesScanningBtn => 'Skanowanie...';

  @override
  String get myDevicesCheckingBtn => 'Sprawdzanie...';

  @override
  String get myDevicesRefreshBtn => 'Odśwież';

  @override
  String get myDevicesDelete => 'Usuń';

  @override
  String get myDevicesNew => 'NOWE';

  @override
  String myDevicesDeviceOffline(String deviceName) {
    return '$deviceName Offline';
  }

  @override
  String get myDevicesLastSeenJustNow => 'Widziany przed chwilą';

  @override
  String myDevicesLastSeenMinutes(int minutes) {
    return 'Widziany $minutes min temu';
  }

  @override
  String myDevicesLastSeenHours(int hours) {
    return 'Widziany $hours godz. temu';
  }

  @override
  String myDevicesLastSeenDays(int days) {
    return 'Widziany $days dni temu';
  }

  @override
  String myDevicesLastSeenWeeks(int weeks) {
    return 'Widziany $weeks tyg. temu';
  }

  @override
  String get ledControlNoConnection => 'Brak połączenia';

  @override
  String get ledControlTryAgain => 'Spróbuj ponownie';

  @override
  String get dashboardTitle => 'Sterowanie LED';

  @override
  String get dashboardConnected => 'Połączono';

  @override
  String get dashboardDisconnected => 'Rozłączono';

  @override
  String get dashboardColor => 'Kolor';

  @override
  String get dashboardBrightness => 'Jasność';

  @override
  String get dashboardEffects => 'Efekty';

  @override
  String get dashboardApply => 'Zastosuj zmiany';

  @override
  String get effectSolid => 'Jednolity';

  @override
  String get effectFade => 'Zanikanie';

  @override
  String get effectRainbow => 'Tęcza';

  @override
  String get effectChase => 'Chase';

  @override
  String get effectTwinkle => 'Migotanie';

  @override
  String get errorBluetoothOff =>
      'Włącz Bluetooth aby połączyć się z PixelTree';

  @override
  String get errorLocationPermission =>
      'Uprawnienie do lokalizacji jest wymagane do skanowania Bluetooth';

  @override
  String get errorBluetoothPermission =>
      'Uprawnienie Bluetooth jest wymagane do połączenia';

  @override
  String get errorDeviceNotFound =>
      'Nie można znaleźć PixelTree. Upewnij się, że jest włączone i w pobliżu.';

  @override
  String get errorConnectionFailed =>
      'Nie udało się połączyć. Spróbujmy ponownie!';

  @override
  String get errorUnknown => 'Coś poszło nie tak. Spróbuj ponownie.';

  @override
  String get categoryStatic => 'Statyczne';

  @override
  String get categoryWave => 'Fale';

  @override
  String get categoryChase => 'Chase';

  @override
  String get categoryTwinkle => 'Twinkle';

  @override
  String get categoryFire => 'Ogień';

  @override
  String get categoryChristmas => 'Świąteczne';

  @override
  String get categorySpecial => 'Specjalne';

  @override
  String get categoryBreathing => 'Breathing';

  @override
  String get categoryAlarm => 'Alarm';

  @override
  String get colorModeWarm => 'Ciepła';

  @override
  String get colorModeCold => 'Zimna';

  @override
  String get colorModeMulti => 'Multi';

  @override
  String get colorModePalette => 'Paleta';

  @override
  String get directionRight => 'Prawo →';

  @override
  String get directionLeft => '← Lewo';

  @override
  String get paramSpeed => 'Prędkość';

  @override
  String get paramColor => 'Kolor';

  @override
  String get paramBrightness => 'Jasność';

  @override
  String get paramIntensity => 'Intensywność';

  @override
  String get paramSize => 'Rozmiar';

  @override
  String get paramWidth => 'Szerokość';

  @override
  String get paramTrail => 'Ogon';

  @override
  String get paramFrequency => 'Częstotliwość';

  @override
  String get paramAmplitude => 'Amplituda';

  @override
  String get paramPalette => 'Paleta';

  @override
  String get paramDirection => 'Kierunek';

  @override
  String get paramStyle => 'Styl';

  @override
  String get paramMode => 'Tryb';

  @override
  String get paramDual => 'Podwójny';

  @override
  String get paramRainbow => 'Tęcza';

  @override
  String get paramSparkle => 'Iskrzenie';

  @override
  String get paramFade => 'Zanikanie';

  @override
  String get paramGap => 'Przerwa';

  @override
  String get paramDots => 'Punkty';

  @override
  String get paramColors => 'Kolory';

  @override
  String get colorPickerTitle => 'Wybierz kolor';
}
