#ifndef COPPER_COMPONENTS_PATH_HPP
#define COPPER_COMPONENTS_PATH_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/json.hpp>
#include <copper/components/request.hpp>
#include <istream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace copper::components {

/**
 * Path from request
 *
 * @param request
 * @return string
 */
std::string path_from_request(const request &request);

}  // namespace copper::components

#endif