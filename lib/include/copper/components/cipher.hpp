#ifndef COPPER_COMPONENTS_CIPHER_HPP
#define COPPER_COMPONENTS_CIPHER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <string>

namespace copper::components {

/**
 * Generate SHA256 key
 *
 * @return string Output
 */
std::string cipher_generate_sha_256();

/**
 * Get HMAC value of input
 *
 * @param input
 * @param app_key
 * @return string Output
 */
std::string cipher_hmac(const std::string &input,
                        const std::string_view &app_key);

/**
 * Generate AES Initialization Vector
 *
 * @return std::pair<std::string, std::string> Pair of secret and initialization
 * vector
 */
std::pair<std::string, std::string> cipher_generate_aes_key_iv();

/**
 * Encrypt
 *
 * @param input
 * @param key
 * @param iv
 *
 * @return string Output
 */
std::string cipher_encrypt(const std::string &input, const std::string &key,
                           const std::string &iv);

/**
 * Decrypt
 *
 * @param input
 * @param key
 * @param iv
 * @return string Output
 */
std::string cipher_decrypt(std::string_view input, const std::string &key,
                           const std::string &iv);

bool cipher_password_validator(const std::string &input,
                               const std::string &hash);

std::string cipher_password_hash(const std::string &input, int workload = 12);

}  // namespace copper::components

#endif