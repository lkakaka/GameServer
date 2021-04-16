package com.util;

import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import java.security.SecureRandom;

public class RC4 {
    private final byte[] S = new byte[256];
    private final byte[] T = new byte[256];
    private final int keylen;

    private SecretKey secretKey;

    public RC4(final byte[] key) {
        if (key.length < 1 || key.length > 256) {
            throw new IllegalArgumentException(
                    "key must be between 1 and 256 bytes");
        } else {
            keylen = key.length;
            for (int i = 0; i < 256; i++) {
                S[i] = (byte) i;
                T[i] = key[i % keylen];
            }
            int j = 0;
            byte tmp;
            for (int i = 0; i < 256; i++) {
                j = (j + S[i] + T[i]) & 0xFF;
                tmp = S[j];
                S[j] = S[i];
                S[i] = tmp;
            }
        }

        initSecretKey(key);
    }

    private void initSecretKey(final byte[] key) {
        try {
            SecureRandom secureRandom = SecureRandom.getInstance("SHA1PRNG");
            secureRandom.setSeed(key);
            KeyGenerator keyGenerator = KeyGenerator.getInstance("RC4");
            keyGenerator.init(secureRandom);
            secretKey = keyGenerator.generateKey();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public byte[] encrypt(final byte[] plaintext) {
        final byte[] ciphertext = new byte[plaintext.length];
        int i = 0, j = 0, k, t;
        byte tmp;
        for (int counter = 0; counter < plaintext.length; counter++) {
            i = (i + 1) & 0xFF;
            j = (j + S[i]) & 0xFF;
            tmp = S[j];
            S[j] = S[i];
            S[i] = tmp;
            t = (S[i] + S[j]) & 0xFF;
            k = S[t];
            ciphertext[counter] = (byte) (plaintext[counter] ^ k);
        }
        return ciphertext;
    }

    public byte[] decrypt(final byte[] ciphertext) {
        return encrypt(ciphertext);
    }

    public byte[] encryptWithRC4(String content) {
//        SecureRandom secureRandom = SecureRandom.getInstance("SHA1PRNG");
//        secureRandom.setSeed(key.getBytes());
//        KeyGenerator keyGenerator = KeyGenerator.getInstance("RC4");
//        keyGenerator.init(secureRandom);
//        SecretKey secretKey = keyGenerator.generateKey();

        try {
            Cipher cipher = Cipher.getInstance("RC4");
            cipher.init(Cipher.ENCRYPT_MODE, secretKey);

            return cipher.doFinal(content.getBytes());
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    public byte[] decryptWithRC4(String cipherText, byte[] cipherBytes) {
//        SecureRandom secureRandom = SecureRandom.getInstance("SHA1PRNG");
//        secureRandom.setSeed(key.getBytes());
//        KeyGenerator keyGenerator = KeyGenerator.getInstance("RC4");
//        keyGenerator.init(secureRandom);
//        SecretKey secretKey = keyGenerator.generateKey();

        try {
            Cipher cipher = Cipher.getInstance("RC4");
            cipher.init(Cipher.DECRYPT_MODE, secretKey);
            byte[] bytes = cipherText.getBytes();

            return cipher.doFinal(cipherText.getBytes());
//            return cipher.doFinal(cipherBytes);
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }
}
