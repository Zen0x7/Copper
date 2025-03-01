#include <copper/components/cache.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/database.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/router.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/state.hpp>
#include <copper/components/views.hpp>

namespace copper::components {

state::state(const shared<boost::mysql::connection_pool>& pool)
    : database_(boost::make_shared<database>(pool)) {
  views::instance()->push("404", "404");
}

shared<database> state::get_database() { return database_; }

}  // namespace copper::components