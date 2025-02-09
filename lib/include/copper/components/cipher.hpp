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
     * Generate SHA256 key
     *
     * @return std::string Output
     */
    std::string generate_sha_256();

    /**
     * Get HMAC value of input
     *
     * @param input
     * @param app_key
     * @return std::string Output
     */
    std::string hmac(
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
    > generate_aes_key_iv();

    /**
     * Encrypt using AES 256 CBC
     * 
     * @param input 
     * @param key 
     * @param iv
     *
     * @return std::string Output
     */
    std::string encrypt_aes_256_cbc(
            const std::string &input,
            const std::string &key,
            const std::string &iv
    );

    /**
     * Decrypt using AES 256 CBC
     *
     * @param input
     * @param key
     * @param iv
     * @return std::string Output
     */
    std::string decrypt_aes_256_cbc(
            const std::string &input,
            const std::string &key,
            const std::string &iv
    );

}  // namespace copper::components::cipher
