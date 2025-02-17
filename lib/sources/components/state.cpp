#include <copper/components/state.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {
    state::state() :
      http_router_(boost::make_shared<http_router>()),
      redis_(boost::make_shared<cache>()),
      database_(boost::make_shared<database>())
    { }

    shared<http_router> state::get_http_router() { return http_router_; }
    shared<cache> state::get_cache() { return redis_; }

    shared<database> state::get_database() {
        return database_;
    }
}