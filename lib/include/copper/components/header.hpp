#ifndef COPPER_COMPONENTS_HEADER_HPP
#define COPPER_COMPONENTS_HEADER_HPP

#pragma once

#include <copper/components/http_request.hpp>
#include <copper/components/http_response.hpp>
#include <copper/components/json.hpp>
#include <string>

namespace copper::components {

/**
 * Retrieves headers from request
 *
 * @param request
 * @return string
 */
std::string header_from_request(const http_request& request);

/**
 * Retrieves headers from response
 *
 * @param response
 * @return string
 */
std::string header_from_response(const http_response& response);

}  // namespace copper::components

#endif