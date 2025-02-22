#ifndef COPPER_COMPONENTS_STATE_HPP
#define COPPER_COMPONENTS_STATE_HPP

#pragma once

#include <copper/components/cache.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/database.hpp>
#include <copper/components/http_router.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {

class state : public shared_enabled<state> {
  /**
   * HTTP router
   */
  shared<http_router> http_router_;

  /**
   * Cache
   */
  shared<cache> redis_;

  /**
   * Database
   */
  shared<database> database_;

 public:
  /**
   * Constructor
   *
   * @param pool
   */
  state(const shared<boost::mysql::connection_pool>& pool);

  /**
   * Get HTTP router
   *
   * @return shared<http_router>
   */
  shared<http_router> get_http_router();

  /**
   * Get cache
   *
   * @return shared<cache>
   */
  shared<cache> get_cache();

  /**
   * Get database
   *
   * @return shared<database>
   */
  shared<database> get_database();
};

}  // namespace copper::components

#endif