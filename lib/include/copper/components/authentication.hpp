#ifndef COPPER_COMPONENTS_AUTHENTICATION_HPP
#define COPPER_COMPONENTS_AUTHENTICATION_HPP

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

#include <boost/algorithm/string.hpp>
#include <boost/json/parse.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <copper/components/base64url.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/json.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/uuid.hpp>
#include <copper/components/validator.hpp>
#include <string>

namespace copper::components {

/**
 * Authentication result
 */
struct authentication_result {
  /**
   * ID
   */
  uuid id_;

  /**
   * Type
   */
  std::string type_;
};

/**
 * Extract token from bearer
 *
 * @param bearer
 * @return std::string
 */
inline std::string authentication_extract_token(const std::string& bearer) {
  return boost::starts_with(bearer, "Bearer ") ? bearer.substr(7) : bearer;
}

/**
 * Split token
 *
 * @param token
 * @return vector_of<string>
 */
inline vector_of<std::string> authentication_split_token(std::string token) {
  vector_of<std::string> parts;
  std::size_t position = 0;
  while ((position = token.find('.')) != std::string::npos) {
    parts.push_back(token.substr(0, position));
    token.erase(0, position + 1);
  }
  parts.push_back(token);
  return parts;
}

/**
 * Verify signature
 *
 * @param parts
 * @param app_key
 * @return bool
 */
inline bool authentication_verify_signature(const vector_of<std::string>& parts,
                                            const std::string& app_key) {
  const std::string merged = parts[0] + "." + parts[1];
  const std::string signature =
      base64url_encode(cipher_hmac(merged, app_key), false);
  return signature == parts[2];
}

/**
 * Parse payload
 *
 * @param encoded
 * @return optional_of<json::value>
 */
inline optional_of<json::value> authentication_parse_payload(
    const std::string& encoded) {
  boost::system::error_code ec;
  auto parsed = boost::json::parse(base64url_decode(encoded), ec);
  if (ec)
    return boost::none;
  return parsed;
}

/**
 * Validate payload
 *
 * @param payload
 * @return bool
 */
inline bool authentication_validate_payload(const json::value& payload) {
  const map_of_strings rules = {
      {"*", "is_object"},   {"sub", "is_uuid"},   {"typ", "is_string"},
      {"iat", "is_number"}, {"exp", "is_number"},
  };

  if (const auto instance = validator_make(rules, payload); !instance->success_)
    return false;

  if (const auto expires_at = payload.as_object().at("exp").as_int64();
      chronos::now() > expires_at)
    return false;

  return true;
}

inline optional_of<authentication_result> authentication_extract_result(
    const json::value& payload) {
  const std::string id{payload.as_object().at("sub").as_string()};
  const std::string type{payload.as_object().at("typ").as_string()};

  LOG("[authentication_from_bearer] id: " << id);
  LOG("[authentication_from_bearer] type: " << type);
  LOG("[authentication_from_bearer] expires_at: "
      << payload.as_object().at("exp").as_int64());

  return authentication_result{
      .id_ = boost::lexical_cast<uuid>(id),
      .type_ = type,
  };
}

/**
 * Authentication from bearer
 *
 * @param bearer
 * @param app_key
 * @return boost::optional<authentication_result> Result
 */
inline optional_of<authentication_result> authentication_from_bearer(
    const std::string& bearer,
    const std::string& app_key) {
  LOG("[authentication_from_bearer] scope_in");

  if (bearer.empty()) {
    LOG("[authentication_from_bearer] scope_out [3 of 3]");
    return boost::none;
  }

  const std::string token = authentication_extract_token(bearer);
  const auto parts = authentication_split_token(token);
  if (parts.size() != 3) {
    LOG("[authentication_from_bearer] scope_out [3 of 3]");
    return boost::none;
  }

  if (!authentication_verify_signature(parts, app_key)) {
    LOG("[authentication_from_bearer] scope_out [3 of 3]");
    return boost::none;
  }

  auto payload = authentication_parse_payload(parts[1]);
  if (!payload.has_value()) {
    LOG("[authentication_from_bearer] scope_out [3 of 3]");
    return boost::none;
  }

  if (!authentication_validate_payload(payload.value())) {
    LOG("[authentication_from_bearer] scope_out [3 of 3]");
    return boost::none;
  }

  return authentication_extract_result(payload.value());
}

/**
 * Authentication to bearer
 *
 * @param id
 * @param app_key
 * @param type
 * @return string
 */
inline std::string authentication_to_bearer(
    const uuid id,
    const std::string& app_key,
    const std::string& type = "App\\Models\\User") {
  const json::object header = {
      {"srv", "Copper"},
      {"aut", "Ian Torres"},
      {"alg", "HS256"},
      {"typ", "JWT"},
  };

  const std::string id_ = to_string(id);
  const auto now = std::chrono::system_clock::now();
  const auto expires_at = now + std::chrono::days(7);
  const auto iat = chronos::to_timestamp(now);
  const auto exp = chronos::to_timestamp(expires_at);
  const json::object payload = {
      {"sub", id_},
      {"typ", type},
      {"iat", iat},
      {"exp", exp},
  };
  const std::string header_ = base64url_encode(serialize(header), false);
  const std::string payload_ = base64url_encode(serialize(payload), false);
  const std::string signature_ =
      base64url_encode(cipher_hmac(header_ + "." + payload_, app_key), false);
  return "Bearer " + header_ + "." + payload_ + "." + signature_;
}

}  // namespace copper::components

#endif
