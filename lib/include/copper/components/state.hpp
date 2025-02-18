#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/shared.hpp>

#include <copper/components/database.hpp>
#include <copper/components/cache.hpp>
#include <copper/components/http_router.hpp>

namespace copper::components {
    class state : public shared_enabled<state> {
        shared<http_router> http_router_;
        shared<cache> redis_;
        shared<database> database_;
    public:
        state(const shared<boost::mysql::connection_pool> & pool);

        shared<http_router> get_http_router();
        shared<cache> get_cache();
        shared<database> get_database();
    };
}