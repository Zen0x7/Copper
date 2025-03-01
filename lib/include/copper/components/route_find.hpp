#ifndef COPPER_COMPONENTS_ROUTE_FIND_HPP
#define COPPER_COMPONENTS_ROUTE_FIND_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/router.hpp>

namespace copper::components {

/**
 * Route find
 *
 * @param url
 * @param route
 * @return route_result
 */
route_result route_find(const std::string &url, const route &route);

}  // namespace copper::components

#endif