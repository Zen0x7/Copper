#include <copper/components/authentication.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/json.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/system.hpp>

#include <copper/components/base64url.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/validator.hpp>

namespace copper::components {

    boost::optional<
            authentication_result
    > authentication_from_bearer(
            const std::string &bearer,
            const std::string &app_key
    ) {
        if (bearer != "") {
            std::string token = boost::starts_with(bearer, "Bearer ") ? bearer.substr(7) : bearer;

            std::vector<std::string> parts;
            std::size_t position = 0;

            while ((position = token.find('.')) != std::string::npos) {
                std::string piece = token.substr(0, position);
                parts.push_back(piece);
                token.erase(0, position + 1);
            }

            parts.push_back(token);

            if (parts.size() == 3) {
                std::string merged = parts[0] + "." + parts[1];
                if (const std::string signature_
                            = base64url::encode(cipher::hmac(merged, app_key), false);
                        signature_ == parts[2]) {
                    boost::system::error_code ec;
                    auto payload = boost::json::parse(base64url::decode(parts[1]), ec);

                    if (!ec) {
                        std::map<std::string, std::string> rules = {
                                {"*",   "is_object"},
                                {"sub", "is_uuid"},
                                {"typ", "is_string"},
                                {"iat", "is_number"},
                                {"exp", "is_number"},
                        };

                        if (auto instance = validator_make(rules, payload); instance->success) {
                            const std::string id{payload.as_object().at("sub").as_string()};
                            const std::string type{payload.as_object().at("typ").as_string()};
                            auto expires_at_ = payload.as_object().at("exp").as_int64();

                            auto id_ = boost::lexical_cast<boost::uuids::uuid>(id);

                            // LCOV_EXCL_START
                            if (
                                auto current_unix = std::chrono::duration_cast<std::chrono::seconds>(
                                        std::chrono::system_clock::now().time_since_epoch()
                                ).count();
                                    current_unix > expires_at_) {
                                return boost::none;
                            }
                            // LCOV_EXCL_STOP

                            return authentication_result {
                                    .id = id_,
                                    .type = type,
                            };
                        }
                    }
                }
            }
        }
        return boost::none;
    }

    std::string authentication_to_bearer(
            const boost::uuids::uuid id,
            const std::string &app_key,
            const std::string &type
    ) {
        const boost::json::object header = {
                {"alg", "HS256"},
                {"typ", "JWT"},
        };

        const std::string id_ = to_string(id);
        const auto now = std::chrono::system_clock::now();
        const auto expires_at = now + std::chrono::days(7);
        const auto iat
                = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        const auto exp
                = std::chrono::duration_cast<std::chrono::seconds>(expires_at.time_since_epoch()).count();
        const boost::json::object payload = {
                {"sub", id_},
                {"typ", type},
                {"iat", iat},
                {"exp", exp},
        };
        const std::string header_ = base64url::encode(serialize(header), false);
        const std::string payload_ = base64url::encode(serialize(payload), false);
        const std::string signature_
                = base64url::encode(cipher::hmac(header_ + "." + payload_, app_key), false);
        return header_ + "." + payload_ + "." + signature_;
    }

}  // namespace copper::components::authenticator
