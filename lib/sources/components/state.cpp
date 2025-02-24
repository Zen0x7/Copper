#include <copper/components/cache.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/database.hpp>
#include <copper/components/router.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/state.hpp>
#include <copper/components/views.hpp>

namespace copper::components {

state::state(const shared<configuration>& configuration,
             const shared<boost::mysql::connection_pool>& pool)
    : configuration_(configuration),
      router_(boost::make_shared<router>()),
      cache_(boost::make_shared<cache>(configuration->shared_from_this())),
      database_(boost::make_shared<database>(pool)),
      views_(boost::make_shared<views>()),
      logger_(boost::make_shared<logger>(configuration->shared_from_this())) {
  this->get_views()->push("404", "404");
}

shared<configuration> state::get_configuration() { return configuration_; }

shared<router> state::get_router() { return router_; }

shared<cache> state::get_cache() { return cache_; }

shared<database> state::get_database() { return database_; }

shared<views> state::get_views() { return views_; }

shared<logger> state::get_logger() { return logger_; }

}  // namespace copper::components