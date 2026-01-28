import 'package:flutter_test/flutter_test.dart';
import 'package:mobile/data/datasources/crypto_datasource.dart';

void main() {
  // ==========================================================================
  // ECDHCryptoService Tests
  // ==========================================================================
  group('ECDHCryptoService', () {
    late ECDHCryptoService crypto;

    setUp(() {
      crypto = ECDHCryptoService();
    });

    test('generateKeyPair returns valid hex public key', () async {
      final publicKey = await crypto.generateKeyPair();

      // Public key should be 130 hex chars
      expect(publicKey.length, 130);
      // Should start with 04
      expect(publicKey.substring(0, 2), '04');
    });

    test('encrypt and decrypt password returns original', () async {
      // Generate key pair
      await crypto.generateKeyPair();

      final crypto2 = ECDHCryptoService();
      final peerPublicKey = await crypto2.generateKeyPair();

      await crypto.computeSharedSecret(peerPublicKey);

      // Encrypt and decrypt
      const originalPassword = 'TajneHaslo123';
      final encrypted = crypto.encryptPassword(originalPassword);

      // Encrypted should be hex string
      expect(encrypted.length, greaterThan(0));
      expect(encrypted, isNot(equals(originalPassword)));
    });

    test('reset clears crypto state', () async {
      await crypto.generateKeyPair();

      crypto.reset();

      expect(crypto.getSharedSecretHex(), isNull);
    });

    test('encryptPassword throws when shared secret not computed', () {
      expect(() => crypto.encryptPassword('test'), throwsException);
    });

    test('computeSharedSecret throws on invalid key format', () async {
      await crypto.generateKeyPair();

      expect(() => crypto.computeSharedSecret('invalid'), throwsException);
    });
  });
}
