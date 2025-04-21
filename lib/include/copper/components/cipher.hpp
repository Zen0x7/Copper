#ifndef COPPER_COMPONENTS_CIPHER_HPP
#define COPPER_COMPONENTS_CIPHER_HPP

#pragma once

// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <bcrypt/BCrypt.hpp>
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
std::string cipher_hmac(const std::string& input,
                        const std::string_view& app_key);

/**
 * Generate AES Initialization Vector
 *
 * @return std::pair<std::string, std::string> Pair of
 * secret and initialization vector
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
std::string cipher_encrypt(const std::string& input,
                           const std::string& key,
                           const std::string& iv);

/**
 * Decrypt
 *
 * @param input
 * @param key
 * @param iv
 * @return string Output
 */
std::string cipher_decrypt(std::string_view input,
                           const std::string& key,
                           const std::string& iv);

/**
 * Validates password
 *
 * @param input
 * @param hash
 * @return bool
 */
inline bool cipher_password_validator(const std::string& input,
                                      const std::string& hash) {
  return BCrypt::validatePassword(input, hash);
}

/**
 * Generate password hash
 *
 * @param input
 * @param workload
 * @return string
 */
inline std::string cipher_password_hash(const std::string& input,
                                        int workload = 12) {
  return BCrypt::generateHash(input, workload);
}

}  // namespace copper::components

#endif
