#pragma once

#include <copper/components/http_route.hpp>
#include <copper/components/http_controller.hpp>
#include <copper/components/http_routes.hpp>
#include <copper/components/http_method.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {
    class http_router : public shared_enabled<http_router> {
        shared<http_routes> routes_;
    public:
        explicit http_router() : routes_(boost::make_shared<http_routes>()) {}

        shared<http_routes> get_routes() {
            return routes_;
        }

        shared<http_router> push(http_method method, const char path[], const shared<http_controller> & controller) {
          get_routes()->push_back(
            std::pair(components::http_router::factory(method, path), controller)
          );
          return shared_from_this();
        }

        static http_route factory(http_method method, const char* path);
    };
}  // namespace copper::components

