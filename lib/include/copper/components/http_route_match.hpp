#ifndef COPPER_COMPONENTS_HTTP_ROUTE_MATCH_HPP
#define COPPER_COMPONENTS_HTTP_ROUTE_MATCH_HPP

#pragma once

#include <copper/components/http_method.hpp>
#include <copper/components/http_router.hpp>

namespace copper::components {
http_route_result http_route_match(http_method method,
                                   const std::string_view &path,
                                   const http_route &route);
}

#endif