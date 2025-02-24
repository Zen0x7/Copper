#ifndef COPPER_COMPONENTS_ROUTE_MATCH_HPP
#define COPPER_COMPONENTS_ROUTE_MATCH_HPP

#pragma once

#include <copper/components/http_method.hpp>
#include <copper/components/router.hpp>

namespace copper::components {

/**
 * Route match
 *
 * @param method
 * @param path
 * @param route
 * @return route_result
 */
route_result route_match(http_method method,
                         const std::string_view &path,
                         const route &route);

}  // namespace copper::components

#endif