#ifndef COPPER_COMPONENTS_QUERY_HPP
#define COPPER_COMPONENTS_QUERY_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/request.hpp>
#include <copper/components/json.hpp>
#include <istream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace copper::components {

/**
 * Query from request
 *
 * @param request
 * @return string
 */
std::string query_from_request(const request &request);

}  // namespace copper::components

#endif