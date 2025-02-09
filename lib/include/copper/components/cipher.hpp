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

namespace copper::components {

    /**
     * Generate SHA256 key
     *
     * @return std::string Output
     */
    std::string cipher_generate_sha_256();

    /**
     * Get HMAC value of input
     *
     * @param input
     * @param app_key
     * @return std::string Output
     */
    std::string cipher_hmac(
            const std::string &input,
            const std::string &app_key
    );

    /**
     * Generate AES Initialization Vector
     *
     * @return std::pair<std::string, std::string> Pair of secret and initialization vector
     */
    std::pair<
            std::string,
            std::string
    > cipher_generate_aes_key_iv();

    /**
     * Encrypt
     * 
     * @param input 
     * @param key 
     * @param iv
     *
     * @return std::string Output
     */
    std::string cipher_encrypt(
            const std::string &input,
            const std::string &key,
            const std::string &iv
    );

    /**
     * Decrypt
     *
     * @param input
     * @param key
     * @param iv
     * @return std::string Output
     */
    std::string cipher_decrypt(
            const std::string &input,
            const std::string &key,
            const std::string &iv
    );

}  // namespace copper::components::cipher
