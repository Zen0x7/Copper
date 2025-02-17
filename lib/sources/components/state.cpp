#include <copper/components/state.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {
    state::state() :
            router_(boost::make_shared<http_router>()),
            redis_(boost::make_shared<redis_service>()),
            database_(boost::make_shared<database>())
    { }

    shared<http_router> state::get_router() { return router_; }
    shared<redis_service> state::get_redis() { return redis_; }

    shared<database> state::get_database() {
        return database_;
    }
}