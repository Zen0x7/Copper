#pragma once

#include <copper/components/http_router.hpp>

namespace copper::components {
http_route_result http_route_find(const std::string_view &path,
                                  const http_route &route);
}