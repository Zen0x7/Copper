#ifndef COPPER_COMPONENTS_HTTP_PATH_HPP
#define COPPER_COMPONENTS_HTTP_PATH_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/http_request.hpp>
#include <copper/components/json.hpp>
#include <istream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace copper::components {

/**
 * HTTP path from request
 *
 * @param request
 * @return string
 */
std::string http_path_from_request(const http_request &request);

}  // namespace copper::components

#endif