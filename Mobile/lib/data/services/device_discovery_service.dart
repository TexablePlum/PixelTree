import 'dart:async';
import 'package:bonsoir/bonsoir.dart';
import '../datasources/http_datasource.dart';
import '../models/discovered_device.dart';

// ============================================================================
// Device Discovery Service - mDNS Service Discovery
// ============================================================================

class DeviceDiscoveryService {
  final HttpDataSource _httpDataSource;

  DeviceDiscoveryService(this._httpDataSource);

  /// Discover ALL PixelTree devices on network via mDNS
  /// Returns list of all found devices after scanning for specified duration
  Future<List<DiscoveredDevice>> discoverAllDevices({
    Duration scanDuration = const Duration(seconds: 8),
  }) async {
    final devices = <String, DiscoveredDevice>{};
    BonsoirDiscovery? discovery;
    StreamSubscription? subscription;

    try {
      discovery = BonsoirDiscovery(type: '_http._tcp');

      // Initialize discovery (required before accessing eventStream)
      await discovery.initialize();

      // Subscribe to events after initialize, before start
      subscription = discovery.eventStream!.listen((event) async {
        switch (event) {
          case BonsoirDiscoveryServiceFoundEvent():
            if (discovery != null) {
              event.service.resolve(discovery.serviceResolver);
            }
            break;

          case BonsoirDiscoveryServiceResolvedEvent():
            final service = event.service;
            final macSuffix = service.attributes['macSuffix'];

            // Only process PixelTree devices (have macSuffix in TXT record)
            if (macSuffix != null && macSuffix.isNotEmpty) {
              final host = service.host ?? '${service.name}.local';

              // Try to get more info from device status
              try {
                final status = await _httpDataSource.getDeviceStatus(host);
                final device = DiscoveredDevice(
                  deviceId: macSuffix,
                  name:
                      status['deviceName'] as String? ?? 'PixelTree-$macSuffix',
                  host: host,
                  ipAddress: status['ip'] as String?,
                  ssid: status['ssid'] as String?,
                );
                devices[macSuffix] = device;
              } catch (e) {
                // Add with basic info if status fetch fails
                final device = DiscoveredDevice(
                  deviceId: macSuffix,
                  name: 'PixelTree-$macSuffix',
                  host: host,
                );
                devices[macSuffix] = device;
              }
            }
            break;

          default:
            break;
        }
      });

      // Start discovery after subscribing to events
      await discovery.start();

      // Wait for scan duration
      await Future.delayed(scanDuration);
    } catch (e) {
      // Ignore errors during scan
    } finally {
      await subscription?.cancel();
      await discovery?.stop();
    }

    return devices.values.toList();
  }

  /// Discover device on network by MAC suffix using mDNS
  /// Returns device info if found, null otherwise
  /// Global timeout: 10 seconds
  Future<Map<String, dynamic>?> discoverDevice(String macSuffix) async {
    try {
      // Try hostname first (fastest - 5s)
      final hostnameResult = await _tryHostname(macSuffix);
      if (hostnameResult != null) {
        return hostnameResult;
      }

      // mDNS service discovery with timeout

      return await _mdnsServiceDiscovery(macSuffix).timeout(
        const Duration(seconds: 10),
        onTimeout: () {
          return null;
        },
      );
    } catch (e) {
      return null;
    }
  }

  /// Try to discover device via hostname (fast path)
  Future<Map<String, dynamic>?> _tryHostname(String macSuffix) async {
    try {
      final hostname = 'pixeltree-${macSuffix.toLowerCase()}.local';

      // Try to get device status via hostname
      final status = await _httpDataSource.getDeviceStatus(hostname);

      // Verify it's the correct device
      if (status['macSuffix'] == macSuffix) {
        return {'host': hostname, 'status': status};
      }
    } catch (e) {
      // Ignore hostname lookup errors
    }

    return null;
  }

  /// mDNS service discovery - finds ALL _http._tcp devices and filters by macSuffix
  Future<Map<String, dynamic>?> _mdnsServiceDiscovery(String macSuffix) async {
    BonsoirDiscovery? discovery;
    StreamSubscription? subscription;

    try {
      // Start discovering _http._tcp services
      discovery = BonsoirDiscovery(type: '_http._tcp');

      // Create completer for async result
      final completer = Completer<Map<String, dynamic>?>();

      // Listen for discovered services
      subscription = discovery.eventStream!.listen((event) async {
        switch (event) {
          case BonsoirDiscoveryServiceFoundEvent():

            // Resolve service to get IP and TXT records
            event.service.resolve(discovery!.serviceResolver);
            break;

          case BonsoirDiscoveryServiceResolvedEvent():
            final service = event.service;

            // Check TXT record for macSuffix
            final txtMacSuffix = service.attributes['macSuffix'];

            if (txtMacSuffix == macSuffix) {
              // Use IP if available, otherwise hostname
              final host = service.host ?? '${service.name}.local';

              // Get device status to confirm
              try {
                final status = await _httpDataSource.getDeviceStatus(host);

                if (!completer.isCompleted) {
                  completer.complete({'host': host, 'status': status});
                }
              } catch (e) {
                // Continue searching other services
              }
            } else {}
            break;

          case BonsoirDiscoveryServiceLostEvent():
            break;

          default:
            break;
        }
      });

      // Start discovery after setting up listener
      await discovery.start();

      // Wait for result or timeout (handled by outer timeout)
      final result = await completer.future;

      // Cleanup
      await subscription.cancel();
      await discovery.stop();

      return result;
    } catch (e) {
      return null;
    } finally {
      // Always clean up resources
      try {
        await subscription?.cancel();
        await discovery?.stop();
      } catch (e) {
        // Ignore cleanup errors
      }
    }
  }
}
