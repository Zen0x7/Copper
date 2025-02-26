#ifndef COPPER_COMPONENTS_ROUTE_MATCH_HPP
#define COPPER_COMPONENTS_ROUTE_MATCH_HPP

#pragma once

#include <copper/components/method.hpp>
#include <copper/components/router.hpp>

namespace copper::components {

/**
 * Route match
 *
 * @param method
 * @param url
 * @param route
 * @return route_result
 */
route_result route_match(method method, const std::string &url,
                         const route &route);

}  // namespace copper::components

#endif