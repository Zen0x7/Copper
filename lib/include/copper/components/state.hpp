#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/shared.hpp>

#include <copper/components/redis_service.hpp>

#include <copper/components/http_router.hpp>

namespace copper::components {
    class state : public shared_enabled<state> {
        shared<http_router> router_;
        shared<redis_service> redis_;
    public:
        state();

        shared<http_router> get_router();
        shared<redis_service> get_redis();
    };
}