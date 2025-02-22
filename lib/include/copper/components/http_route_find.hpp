#ifndef COPPER_COMPONENTS_HTTP_ROUTE_FIND_HPP
#define COPPER_COMPONENTS_HTTP_ROUTE_FIND_HPP

#pragma once

#include <copper/components/http_router.hpp>

namespace copper::components {

/**
 * HTTP route find
 *
 * @param path
 * @param route
 * @return http_route_result
 */
http_route_result http_route_find(const std::string_view &path,
                                  const http_route &route);

}  // namespace copper::components

#endif