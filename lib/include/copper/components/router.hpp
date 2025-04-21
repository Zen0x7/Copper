#ifndef COPPER_COMPONENTS_ROUTER_HPP
#define COPPER_COMPONENTS_ROUTER_HPP

#pragma once

// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <copper/components/controller.hpp>
#include <copper/components/controller_configuration.hpp>
#include <copper/components/method.hpp>
#include <copper/components/route.hpp>
#include <copper/components/routes.hpp>

namespace copper::components {

/**
 * Router
 */
class router : public shared_enabled<router> {
 public:
  /**
   * Routes
   */
  shared<routes> routes_ = boost::make_shared<routes>();

  /**
   * Constructor
   */
  explicit router();

  /**
   * Retrieves routes
   *
   * @return shared<routes>
   */
  shared<routes> get_routes() { return routes_; }

  /**
   * Push
   *
   * @param method
   * @param path
   * @param controller
   * @param controller_configuration
   * @return shared<router>
   */
  shared<router> push(method method,
                      const char path[],
                      const shared<controller>& controller,
                      controller_configuration controller_configuration);

  /**
   * Make route
   *
   * @param method
   * @param path
   * @return route
   */
  static route make_route(method method, const char* path);

  /**
   * Factory
   *
   * @return shared<router>
   */
  static shared<router> factory();
};

}  // namespace copper::components

#endif
