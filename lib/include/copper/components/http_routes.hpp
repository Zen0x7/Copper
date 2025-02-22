#ifndef COPPER_COMPONENTS_HTTP_ROUTES_HPP
#define COPPER_COMPONENTS_HTTP_ROUTES_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/http_controller.hpp>
#include <copper/components/http_route.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {

/**
 * HTTP routes
 */
typedef containers::vector_of_shared_pairs<http_route, http_controller>
    http_routes;

}  // namespace copper::components

#endif