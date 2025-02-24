#ifndef COPPER_COMPONENTS_HEADER_HPP
#define COPPER_COMPONENTS_HEADER_HPP

#pragma once

#include <copper/components/json.hpp>
#include <copper/components/request.hpp>
#include <copper/components/response.hpp>
#include <string>

namespace copper::components {

/**
 * Retrieves headers from request
 *
 * @param request
 * @return string
 */
std::string header_from_request(const request& request);

/**
 * Retrieves headers from response
 *
 * @param response
 * @return string
 */
std::string header_from_response(const response& response);

}  // namespace copper::components

#endif