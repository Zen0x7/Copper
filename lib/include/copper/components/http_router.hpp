#ifndef COPPER_COMPONENTS_HTTP_ROUTER_HPP
#define COPPER_COMPONENTS_HTTP_ROUTER_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/http_controller.hpp>
#include <copper/components/http_controller_configuration.hpp>
#include <copper/components/http_method.hpp>
#include <copper/components/http_route.hpp>
#include <copper/components/http_routes.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {

/**
 * HTTP router
 */
class http_router : public shared_enabled<http_router> {
  /**
   * Routes
   */
  shared<http_routes> routes_;

 public:
  /**
   * Constructor
   */
  explicit http_router();

  /**
   * Retrieves routes
   *
   * @return shared<http_routes>
   */
  shared<http_routes> get_routes();

  /**
   * Push
   *
   * @param method
   * @param path
   * @param controller
   * @param config
   * @return shared<http_router>
   */
  shared<http_router> push(http_method method, const char path[],
                           const shared<http_controller>& controller,
                           http_controller_configuration config);

  /**
   * HTTP router factory
   *
   * @param method
   * @param path
   * @return http_route
   */
  static http_route factory(http_method method, const char* path);
};

}  // namespace copper::components

#endif
