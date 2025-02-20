#ifndef COPPER_COMPONENTS_AUTHENTICATION_HPP
#define COPPER_COMPONENTS_AUTHENTICATION_HPP

#pragma once

#include <boost/optional.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/uuid.hpp>
#include <string>

namespace copper::components {
struct authentication_result {
  uuid id;
  std::string type;
};

/**
 * Decode JWT Token
 *
 * @param bearer
 * @param app_key
 * @return boost::optional<authentication_result> Result
 */
containers::optional_of<authentication_result> authentication_from_bearer(
    const std::string &bearer, const std::string &app_key);

/**
 * Issue JWT Token
 *
 * @param id
 * @param app_key
 * @param type
 * @return
 */
std::string authentication_to_bearer(
    uuid id, const std::string &app_key,
    const std::string &type = "App\\Models\\User");

}  // namespace copper::components

#endif