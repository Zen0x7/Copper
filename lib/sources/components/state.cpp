#include <copper/components/cache.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/database.hpp>
#include <copper/components/http_router.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/state.hpp>
#include <copper/components/views.hpp>

namespace copper::components {

state::state(const shared<configuration>& configuration,
             const shared<boost::mysql::connection_pool>& pool)
    : configuration_(configuration),
      http_router_(boost::make_shared<http_router>()),
      cache_(boost::make_shared<cache>(configuration)),
      database_(boost::make_shared<database>(pool)),
      views_(boost::make_shared<views>()),
      logger_(boost::make_shared<logger>()) {
  this->get_views()->push("404", "404");
}

shared<configuration> state::get_configuration() { return configuration_; }

shared<http_router> state::get_http_router() { return http_router_; }

shared<cache> state::get_cache() { return cache_; }

shared<database> state::get_database() { return database_; }

shared<views> state::get_views() { return views_; }

shared<logger> state::get_logger() { return logger_; }

}  // namespace copper::components