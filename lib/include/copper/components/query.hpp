#ifndef COPPER_COMPONENTS_QUERY_HPP
#define COPPER_COMPONENTS_QUERY_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

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
 * Query from request
 *
 * @param request
 * @return string
 */
std::string query_from_request(const request &request);

}  // namespace copper::components

#endif