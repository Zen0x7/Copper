#ifndef COPPER_COMPONENTS_ROUTER_HPP
#define COPPER_COMPONENTS_ROUTER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

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
  /**
   * Routes
   */
  shared<routes> routes_ = boost::make_shared<routes>();

 public:
  /**
   * Constructor
   */
  explicit router();

  /**
   * Retrieves routes
   *
   * @return shared<routes>
   */
  shared<routes> get_routes();

  /**
   * Push
   *
   * @param method
   * @param path
   * @param controller
   * @param config
   * @return shared<router>
   */
  shared<router> push(method method, const char path[],
                      const shared<controller>& controller,
                      controller_configuration config);

  /**
   * Router factory
   *
   * @param method
   * @param path
   * @return route
   */
  static route factory(method method, const char* path);

  /**
   * Get instance
   *
   * @return shared<router>
   */
  static shared<router> instance();

 private:
  /**
   * Instance
   */
  static shared<router> instance_;

  /**
   * Initialization flag
   */
  static std::once_flag initialization_flag_;
};

}  // namespace copper::components

#endif
