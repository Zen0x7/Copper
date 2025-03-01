#ifndef COPPER_COMPONENTS_ROUTES_HPP
#define COPPER_COMPONENTS_ROUTES_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/containers.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/route.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {

/**
 * Routes
 */
typedef containers::vector_of_shared_pairs<route, controller> routes;

}  // namespace copper::components

#endif