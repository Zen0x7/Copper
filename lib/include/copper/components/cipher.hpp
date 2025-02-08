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

    /**
     * Generate SHA256 secret key
     *
     * @return
     */
    static std::string generate_sha_256() {
        unsigned char bytes[32];
        std::string hex_key;
        if (RAND_bytes(bytes, sizeof(bytes)) != 1) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            throw std::runtime_error(error_output.c_str());
        } else {
            // LCOV_EXCL_STOP
            std::stringstream ss;
            for (const auto byte : bytes) {
                ss << byte;
            }
            return base64::encode(ss.str());
        }
    }

    /**
     * Get HMAC value of input
     *
     * @param input
     * @param app_key
     * @return
     */
    static std::string hmac(const std::string &input, const std::string &app_key) {
        std::string result;

        EVP_PKEY *pkey = EVP_PKEY_new_mac_key(
                EVP_PKEY_HMAC, NULL, reinterpret_cast<const unsigned char *>(app_key.data()), 32);
        if (!pkey) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        if (!ctx) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            EVP_PKEY_free(pkey);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        std::size_t len;
        unsigned char md[EVP_MAX_MD_SIZE];

        if (EVP_DigestSignInit(ctx, NULL, EVP_sha256(), NULL, pkey) != 1) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            EVP_MD_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        if (EVP_DigestSignUpdate(ctx, input.c_str(), input.size()) != 1) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            EVP_MD_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        if (EVP_DigestSignFinal(ctx, NULL, &len) != 1) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            EVP_MD_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        if (EVP_DigestSignFinal(ctx, md, &len) != 1) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            EVP_MD_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(pkey);

        result.assign(reinterpret_cast<char *>(md), len);
        return result; }

    /**
     * Generate AES Initialization Vector
     * @return
     */
    static std::pair<std::string, std::string> generate_aes_key_iv() {
        std::pair<std::string, std::string> key_iv;

        std::vector<unsigned char> key(32);
        if (RAND_bytes(key.data(), key.size()) != 1) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        key_iv.first.assign(key.begin(), key.end());

        std::vector<unsigned char> iv(EVP_MAX_IV_LENGTH);
        if (RAND_bytes(iv.data(), iv.size()) != 1) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        key_iv.second.assign(iv.begin(), iv.end());

        return key_iv; }

    /**
     * Encrypt using AES 256 CBC
     * 
     * @param input 
     * @param key 
     * @param iv 
     * @return 
     */
    static std::string encrypt_aes_256_cbc(const std::string &input, const std::string &key,
                                           const std::string &iv) {
        std::string output;

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                               reinterpret_cast<const unsigned char *>(key.c_str()),
                               reinterpret_cast<const unsigned char *>(iv.c_str()))
            != 1) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        int length;
        int output_length;
        unsigned char *output_buffer
                = new unsigned char[input.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc())];

        if (EVP_EncryptUpdate(ctx, output_buffer, &length,
                              reinterpret_cast<const unsigned char *>(input.c_str()),
                              input.size())
            != 1) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        output_length = length;

        if (EVP_EncryptFinal_ex(ctx, output_buffer + length, &length) != 1) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        output_length += length;

        output.assign(reinterpret_cast<char *>(output_buffer), output_length);

        delete[] output_buffer;
        EVP_CIPHER_CTX_free(ctx);

        return output; }

    static std::string decrypt_aes_256_cbc(const std::string &input, const std::string &key,
                                           const std::string &iv) {
        std::string output;

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                               reinterpret_cast<const unsigned char *>(key.c_str()),
                               reinterpret_cast<const unsigned char *>(iv.c_str()))
            != 1) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        int length;
        int input_length;
        unsigned char *input_buffer
                = new unsigned char[input.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc())];

        if (EVP_DecryptUpdate(ctx, input_buffer, &length,
                              reinterpret_cast<const unsigned char *>(input.c_str()),
                              input.size())
            != 1) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        input_length = length;

        if (EVP_DecryptFinal_ex(ctx, input_buffer + length, &length) != 1) {
            // LCOV_EXCL_START
            unsigned long error_code = ERR_get_error();
            char error_message[256];
            ERR_error_string_n(error_code, error_message, sizeof(error_message));
            std::string error_output = "OpenSSL error: ";
            error_output.append(error_message);
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error(error_output.c_str());
        }
        // LCOV_EXCL_STOP

        input_length += length;

        output.assign(reinterpret_cast<char *>(input_buffer), input_length);

        delete[] input_buffer;
        EVP_CIPHER_CTX_free(ctx);

        return output; }
}  // namespace copper::components::cipher
