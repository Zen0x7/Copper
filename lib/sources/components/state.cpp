#include <copper/components/state.hpp>

namespace copper::components {
    state::state() :
            router_(boost::make_shared<http_router>()),
            redis_(boost::make_shared<redis_service>())
    {}

    shared<http_router> state::get_router() { return router_; }
    shared<redis_service> state::get_redis() { return redis_; }
}