#pragma once

#include <boost/optional.hpp>
#include <boost/uuid.hpp>
#include <string>

namespace copper::components::authenticator {
    struct result {
        boost::uuids::uuid id;
        std::string type;
    };

    /**
     * Decode JWT Token
     *
     * @param bearer
     * @param app_key
     * @return boost::optional<result> Result
     */
    boost::optional<result> from_bearer(const std::string &bearer, const std::string &app_key);

    /**
     * Issue JWT Token
     *
     * @param id
     * @param app_key
     * @param type
     * @return
     */
    std::string to_bearer(boost::uuids::uuid id, const std::string &app_key, const std::string &type = "App\\Models\\User");

}  // namespace copper::components::authenticator
