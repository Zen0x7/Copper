#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/http_route.hpp>
#include <copper/components/http_controller.hpp>

namespace copper::components {
    typedef containers::vector_of_shared_pairs<http_route, http_controller> http_routes;
}