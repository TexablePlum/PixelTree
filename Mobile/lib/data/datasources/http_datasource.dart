import 'package:dio/dio.dart';

// ============================================================================
// HTTP Datasource - ESP32 Communication
// ============================================================================

class HttpDataSource {
  final Dio _dio;

  HttpDataSource()
    : _dio = Dio(
        BaseOptions(
          connectTimeout: const Duration(seconds: 5),
          receiveTimeout: const Duration(seconds: 5),
          headers: {'Content-Type': 'application/json'},
        ),
      );

  /// Get device status from ESP32
  /// Returns: {deviceName, macSuffix, ip, ssid, connected, state, provisioned}
  Future<Map<String, dynamic>> getDeviceStatus(String host) async {
    try {
      final response = await _dio.get('http://$host/api/status');

      if (response.statusCode == 200 && response.data is Map) {
        return Map<String, dynamic>.from(response.data);
      } else {
        throw Exception('Invalid response from device');
      }
    } on DioException catch (e) {
      if (e.type == DioExceptionType.connectionTimeout ||
          e.type == DioExceptionType.receiveTimeout) {
        throw Exception('Device timeout - may be offline');
      } else if (e.type == DioExceptionType.connectionError) {
        throw Exception('Cannot reach device');
      } else {
        throw Exception('HTTP error: ${e.message}');
      }
    } catch (e) {
      throw Exception('Unexpected error: $e');
    }
  }

  void dispose() {
    _dio.close();
  }
}
