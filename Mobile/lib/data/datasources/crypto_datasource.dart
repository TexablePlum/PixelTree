import 'dart:typed_data';
import 'package:pointycastle/export.dart';
import 'dart:convert';
import 'package:convert/convert.dart';

// ============================================================================
// ECDH Crypto Service - P-256 Key Exchange + AES-128 Encryption
// ============================================================================
// Compatible with ESP32 mbedtls implementation

class ECDHCryptoService {
  // P-256 curve parameters
  static final ECDomainParameters _ecParams = ECDomainParameters('prime256v1');

  // Our key pair
  AsymmetricKeyPair<ECPublicKey, ECPrivateKey>? _keyPair;
  Uint8List? _sharedSecret;

  // ========================================================================
  // Key Generation
  // ========================================================================

  /// Generate new ECDH key pair (P-256)
  Future<String> generateKeyPair() async {
    final keyGen = ECKeyGenerator();

    final secureRandom = FortunaRandom();
    final seedSource = Uint8List.fromList(
      List<int>.generate(
        32,
        (i) => DateTime.now().millisecondsSinceEpoch % 256,
      ),
    );
    secureRandom.seed(KeyParameter(seedSource));

    keyGen.init(
      ParametersWithRandom(ECKeyGeneratorParameters(_ecParams), secureRandom),
    );

    final generatedKeyPair = keyGen.generateKeyPair();
    _keyPair = AsymmetricKeyPair<ECPublicKey, ECPrivateKey>(
      generatedKeyPair.publicKey as ECPublicKey,
      generatedKeyPair.privateKey as ECPrivateKey,
    );

    // Export public key as uncompressed point (65 bytes)
    return _exportPublicKey();
  }

  /// Export public key as hex string (130 chars = 65 bytes)
  String _exportPublicKey() {
    if (_keyPair == null) {
      throw Exception('Key pair not generated');
    }

    final publicKey = _keyPair!.publicKey;
    final point = publicKey.Q!;

    // Encode as uncompressed point: 0x04 + X + Y
    final x = point.x!.toBigInteger()!.toUnsigned(256);
    final y = point.y!.toBigInteger()!.toUnsigned(256);

    final xBytes = _bigIntToBytes(x, 32);
    final yBytes = _bigIntToBytes(y, 32);

    final publicKeyBytes = Uint8List(65);
    publicKeyBytes[0] = 0x04; // Uncompressed point marker
    publicKeyBytes.setRange(1, 33, xBytes);
    publicKeyBytes.setRange(33, 65, yBytes);

    final hexKey = hex.encode(publicKeyBytes).toUpperCase();
    return hexKey;
  }

  // ========================================================================
  // Shared Secret Computation
  // ========================================================================

  /// Compute shared secret from peer's public key
  Future<void> computeSharedSecret(String peerPublicKeyHex) async {
    if (_keyPair == null) {
      throw Exception('Key pair not generated');
    }

    // Decode hex string to bytes (65 bytes)
    final peerPublicKeyBytes = Uint8List.fromList(
      hex.decode(peerPublicKeyHex.toLowerCase()),
    );

    if (peerPublicKeyBytes.length != 65 || peerPublicKeyBytes[0] != 0x04) {
      throw Exception('Invalid public key format');
    }

    // Extract X and Y coordinates
    final xBytes = peerPublicKeyBytes.sublist(1, 33);
    final yBytes = peerPublicKeyBytes.sublist(33, 65);

    final x = _bytesToBigInt(xBytes);
    final y = _bytesToBigInt(yBytes);

    // Create EC point
    final point = _ecParams.curve.createPoint(x, y);

    // Create peer's public key
    final peerPublicKey = ECPublicKey(point, _ecParams);

    // Compute shared secret
    final agreement = ECDHBasicAgreement();
    agreement.init(_keyPair!.privateKey);

    final sharedSecretBigInt = agreement.calculateAgreement(peerPublicKey);
    _sharedSecret = _bigIntToBytes(sharedSecretBigInt, 32);
  }

  // ========================================================================
  // AES Encryption/Decryption
  // ========================================================================

  /// Encrypt WiFi password using AES-128
  /// Uses first 16 bytes of shared secret as key
  String encryptPassword(String password) {
    if (_sharedSecret == null) {
      throw Exception('Shared secret not computed');
    }

    // Use first 16 bytes of shared secret for AES-128
    final aesKey = _sharedSecret!.sublist(0, 16);

    // Add PKCS#7 padding
    final passwordBytes = utf8.encode(password);
    final paddedData = _addPKCS7Padding(passwordBytes, 16);

    // Initialize AES cipher
    final cipher = AESEngine()..init(true, KeyParameter(aesKey));

    // Encrypt in ECB mode (matching ESP32 implementation)
    final encryptedBytes = Uint8List(paddedData.length);
    for (var i = 0; i < paddedData.length; i += 16) {
      cipher.processBlock(paddedData, i, encryptedBytes, i);
    }

    return hex.encode(encryptedBytes).toUpperCase();
  }

  /// Decrypt password using AES-128 (for testing/verification)
  String decryptPassword(String encryptedHex) {
    if (_sharedSecret == null) {
      throw Exception('Shared secret not computed');
    }

    final aesKey = _sharedSecret!.sublist(0, 16);
    final encryptedBytes = Uint8List.fromList(
      hex.decode(encryptedHex.toLowerCase()),
    );

    final cipher = AESEngine()..init(false, KeyParameter(aesKey));

    final decryptedBytes = Uint8List(encryptedBytes.length);
    for (var i = 0; i < encryptedBytes.length; i += 16) {
      cipher.processBlock(encryptedBytes, i, decryptedBytes, i);
    }

    // Remove PKCS#7 padding
    final unpaddedData = _removePKCS7Padding(decryptedBytes);

    return utf8.decode(unpaddedData);
  }

  // ========================================================================
  // Helper Methods
  // ========================================================================

  /// Convert BigInt to fixed-size byte array
  Uint8List _bigIntToBytes(BigInt number, int size) {
    final bytes = Uint8List(size);
    var temp = number;

    for (var i = size - 1; i >= 0; i--) {
      bytes[i] = (temp & BigInt.from(0xFF)).toInt();
      temp = temp >> 8;
    }

    return bytes;
  }

  /// Convert byte array to BigInt
  BigInt _bytesToBigInt(Uint8List bytes) {
    BigInt result = BigInt.zero;
    for (var byte in bytes) {
      result = (result << 8) | BigInt.from(byte);
    }
    return result;
  }

  /// Add PKCS#7 padding to data
  Uint8List _addPKCS7Padding(List<int> dataList, int blockSize) {
    final data = Uint8List.fromList(dataList);
    final paddingLength = blockSize - (data.length % blockSize);

    final paddedData = Uint8List(data.length + paddingLength);
    paddedData.setRange(0, data.length, data);

    for (var i = data.length; i < paddedData.length; i++) {
      paddedData[i] = paddingLength;
    }

    return paddedData;
  }

  /// Remove PKCS#7 padding from data
  Uint8List _removePKCS7Padding(Uint8List data) {
    if (data.isEmpty) {
      throw Exception('Empty data');
    }

    final paddingLength = data.last;

    if (paddingLength < 1 || paddingLength > 16) {
      throw Exception('Invalid padding');
    }

    return data.sublist(0, data.length - paddingLength);
  }

  /// Get shared secret for debugging (hex string)
  String? getSharedSecretHex() {
    return _sharedSecret != null ? hex.encode(_sharedSecret!) : null;
  }

  /// Reset crypto state
  void reset() {
    _keyPair = null;
    _sharedSecret = null;
  }
}
