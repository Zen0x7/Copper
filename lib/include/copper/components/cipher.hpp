#pragma once

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <copper/components/base64.hpp>
#include <copper/components/base64url.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace copper::components::cipher {
    static std::string generate_sha_256() {
        unsigned char bytes[32];
        std::string hex_key;
        if (RAND_bytes(bytes, sizeof(bytes)) != 1) {
        } else {
            std::stringstream ss;
            for (const auto byte : bytes) {
                ss << byte;
            }
            return base64::encode(ss.str());
        }
        return "";
    }

    static std::string hmac(const std::string &input, const std::string &app_key) {
        std::string result;

        EVP_PKEY *pkey = EVP_PKEY_new_mac_key(
                EVP_PKEY_HMAC, NULL, reinterpret_cast<const unsigned char *>(app_key.data()), 32);
        if (!pkey) {
            std::cerr << "Error creating EVP_PKEY" << std::endl;
            return result;
        }

        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        if (!ctx) {
            std::cerr << "Error creating EVP_MD_CTX" << std::endl;
            EVP_PKEY_free(pkey);
            return result;
        }

        std::size_t len;
        unsigned char md[EVP_MAX_MD_SIZE];

        if (EVP_DigestSignInit(ctx, NULL, EVP_sha256(), NULL, pkey) != 1) {
            std::cerr << "Error initializing signing" << std::endl;
            EVP_MD_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            return result;
        }

        if (EVP_DigestSignUpdate(ctx, input.c_str(), input.size()) != 1) {
            std::cerr << "Error updating signing" << std::endl;
            EVP_MD_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            return result;
        }

        if (EVP_DigestSignFinal(ctx, NULL, &len) != 1) {
            std::cerr << "Error finalizing signing" << std::endl;
            EVP_MD_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            return result;
        }

        if (EVP_DigestSignFinal(ctx, md, &len) != 1) {
            std::cerr << "Error finalizing signing" << std::endl;
            EVP_MD_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            return result;
        }

        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(pkey);

        result.assign(reinterpret_cast<char *>(md), len);
        return result;
    }

    static std::pair<std::string, std::string> generate_aes_key_iv() {
        std::pair<std::string, std::string> key_iv;

        // Generate AES key
        std::vector<unsigned char> aesKey(32);
        if (RAND_bytes(aesKey.data(), aesKey.size()) != 1) {
            std::cerr << "Error generating AES key" << std::endl;
            return key_iv;
        }
        key_iv.first.assign(aesKey.begin(), aesKey.end());

        // Generate IV (Initialization Vector)
        std::vector<unsigned char> iv(EVP_MAX_IV_LENGTH);
        if (RAND_bytes(iv.data(), iv.size()) != 1) {
            std::cerr << "Error generating IV" << std::endl;
            return key_iv;
        }
        key_iv.second.assign(iv.begin(), iv.end());

        return key_iv;
    }

    static std::string encrypt_aes_256_cbc(const std::string &plaintext, const std::string &key,
                                           const std::string &iv) {
        std::string ciphertext;

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            std::cerr << "Error creating EVP_CIPHER_CTX" << std::endl;
            return ciphertext;
        }

        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                               reinterpret_cast<const unsigned char *>(key.c_str()),
                               reinterpret_cast<const unsigned char *>(iv.c_str()))
            != 1) {
            std::cerr << "Error initializing AES-256-CBC encryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return ciphertext;
        }

        // Provide plaintext to be encrypted
        int len;
        int ciphertextLen;
        unsigned char *ciphertextBuf
                = new unsigned char[plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc())];

        if (EVP_EncryptUpdate(ctx, ciphertextBuf, &len,
                              reinterpret_cast<const unsigned char *>(plaintext.c_str()),
                              plaintext.size())
            != 1) {
            std::cerr << "Error performing AES-256-CBC encryption" << std::endl;
            delete[] ciphertextBuf;
            EVP_CIPHER_CTX_free(ctx);
            return ciphertext;
        }
        ciphertextLen = len;

        if (EVP_EncryptFinal_ex(ctx, ciphertextBuf + len, &len) != 1) {
            std::cerr << "Error finalizing AES-256-CBC encryption" << std::endl;
            delete[] ciphertextBuf;
            EVP_CIPHER_CTX_free(ctx);
            return ciphertext;
        }
        ciphertextLen += len;

        ciphertext.assign(reinterpret_cast<char *>(ciphertextBuf), ciphertextLen);

        delete[] ciphertextBuf;
        EVP_CIPHER_CTX_free(ctx);

        return ciphertext;
    }

    static std::string decrypt_aes_256_cbc(const std::string &ciphertext, const std::string &key,
                                           const std::string &iv) {
        std::string decryptedData;

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            std::cerr << "Error creating EVP_CIPHER_CTX" << std::endl;
            return decryptedData;
        }

        if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                               reinterpret_cast<const unsigned char *>(key.c_str()),
                               reinterpret_cast<const unsigned char *>(iv.c_str()))
            != 1) {
            std::cerr << "Error initializing AES-256-CBC decryption" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return decryptedData;
        }

        int len;
        int plaintextLen;
        unsigned char *plaintextBuf
                = new unsigned char[ciphertext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc())];

        if (EVP_DecryptUpdate(ctx, plaintextBuf, &len,
                              reinterpret_cast<const unsigned char *>(ciphertext.c_str()),
                              ciphertext.size())
            != 1) {
            std::cerr << "Error performing AES-256-CBC decryption" << std::endl;
            delete[] plaintextBuf;
            EVP_CIPHER_CTX_free(ctx);
            return decryptedData;
        }
        plaintextLen = len;

        if (EVP_DecryptFinal_ex(ctx, plaintextBuf + len, &len) != 1) {
            std::cerr << "Error finalizing AES-256-CBC decryption" << std::endl;
            delete[] plaintextBuf;
            EVP_CIPHER_CTX_free(ctx);
            return decryptedData;
        }
        plaintextLen += len;

        decryptedData.assign(reinterpret_cast<char *>(plaintextBuf), plaintextLen);

        delete[] plaintextBuf;
        EVP_CIPHER_CTX_free(ctx);

        return decryptedData;
    }
}  // namespace copper::components::cipher
