#ifndef COPPER_COMPONENTS_ROUTER_HPP
#define COPPER_COMPONENTS_ROUTER_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/controller_configuration.hpp>
#include <copper/components/method.hpp>
#include <copper/components/route.hpp>
#include <copper/components/routes.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {

/**
 * Router
 */
class router : public shared_enabled<router> {
  /**
   * Routes
   */
  shared<routes> routes_;

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
};

}  // namespace copper::components

#endif
