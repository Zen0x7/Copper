//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/algorithm/string.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>
#include <boost/lexical_cast.hpp>
#include <copper/components/authentication.hpp>
#include <copper/components/base64url.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/validator.hpp>

namespace copper::components {

boost::optional<authentication_result> authentication_from_bearer(
    const std::string &bearer, const std::string &app_key) {
  if (bearer != "") {
    std::string _token =
        boost::starts_with(bearer, "Bearer ") ? bearer.substr(7) : bearer;

    containers::vector_of<std::string> _parts;
    std::size_t _position = 0;

    while ((_position = _token.find('.')) != std::string::npos) {
      std::string _piece = _token.substr(0, _position);
      _parts.push_back(_piece);
      _token.erase(0, _position + 1);
    }

    _parts.push_back(_token);

    if (_parts.size() == 3) {
      std::string _merged = _parts[0] + "." + _parts[1];
      if (const std::string _signature =
              base64url_encode(cipher_hmac(_merged, app_key), false);
          _signature == _parts[2]) {
        boost::system::error_code _ec;
        auto _payload = boost::json::parse(base64url_decode(_parts[1]), _ec);

        if (!_ec) {
          containers::map_of_strings _rules = {
              {"*", "is_object"},   {"sub", "is_uuid"},   {"typ", "is_string"},
              {"iat", "is_number"}, {"exp", "is_number"},
          };

          if (auto _instance = validator_make(_rules, _payload);
              _instance->success_) {
            const std::string _id{_payload.as_object().at("sub").as_string()};
            const std::string _type{_payload.as_object().at("typ").as_string()};
            auto _expires_at = _payload.as_object().at("exp").as_int64();

            auto _id_ = boost::lexical_cast<boost::uuids::uuid>(_id);

            // LCOV_EXCL_START
            if (auto _current_unix = chronos::now();
                _current_unix > _expires_at) {
              return boost::none;
            }
            // LCOV_EXCL_STOP

            return authentication_result{
                .id_ = _id_,
                .type_ = _type,
            };
          }
        }
      }
    }
  }
  return boost::none;
}

std::string authentication_to_bearer(const boost::uuids::uuid id,
                                     const std::string &app_key,
                                     const std::string &type) {
  const boost::json::object header = {
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
  const boost::json::object payload = {
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
