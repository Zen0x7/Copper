#ifndef COPPER_COMPONENTS_ROUTES_HPP
#define COPPER_COMPONENTS_ROUTES_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/http_controller.hpp>
#include <copper/components/route.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {

/**
 * Routes
 */
typedef containers::vector_of_shared_pairs<route, http_controller> routes;

}  // namespace copper::components

#endif