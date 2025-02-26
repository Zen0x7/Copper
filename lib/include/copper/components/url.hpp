#ifndef COPPER_COMPONENTS_URL_HPP
#define COPPER_COMPONENTS_URL_HPP

#pragma once

#include <copper/components/request.hpp>

namespace copper::components {
/**
 * URL from request
 * @param request
 * @return string
 */
std::string url_from_request(const request& request);
}  // namespace copper::components

#endif