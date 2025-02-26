#ifndef COPPER_COMPONENTS_ROUTE_FIND_HPP
#define COPPER_COMPONENTS_ROUTE_FIND_HPP

#pragma once

#include <copper/components/router.hpp>

namespace copper::components {

/**
 * Route find
 *
 * @param path
 * @param route
 * @return route_result
 */
route_result route_find(const std::string &url, const route &route);

}  // namespace copper::components

#endif