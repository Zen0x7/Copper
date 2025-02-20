#ifndef COPPER_COMPONENTS_HTTP_ROUTER_HPP
#define COPPER_COMPONENTS_HTTP_ROUTER_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/http_controller.hpp>
#include <copper/components/http_controller_config.hpp>
#include <copper/components/http_method.hpp>
#include <copper/components/http_route.hpp>
#include <copper/components/http_routes.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {

class http_router : public shared_enabled<http_router> {
  shared<http_routes> routes_;

 public:
  explicit http_router();

  shared<http_routes> get_routes();

  shared<http_router> push(http_method method, const char path[],
                           const shared<http_controller>& controller,
                           http_controller_config config);

  static http_route factory(http_method method, const char* path);
};

}  // namespace copper::components

#endif
