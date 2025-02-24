#ifndef COPPER_COMPONENTS_STATE_HPP
#define COPPER_COMPONENTS_STATE_HPP

#pragma once

#include <boost/mysql/connection_pool.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {

/**
 * Forward cache
 */
class cache;

/**
 * Forward HTTP router
 */
class http_router;

/**
 * Forward configuration
 */
class configuration;

/**
 * Forward database
 */
class database;

/**
 * Forward views
 */
class views;

/**
 * Forward logger
 */
class logger;

/**
 * State
 */
class state : public shared_enabled<state> {
  /**
   *
   */
  shared<configuration> configuration_;

  /**
   * HTTP router
   */
  shared<http_router> http_router_;

  /**
   * Cache
   */
  shared<cache> cache_;

  /**
   * Database
   */
  shared<database> database_;

  /**
   * Views
   */
  shared<views> views_;

  /**
   * Views
   */
  shared<logger> logger_;

 public:
  /**
   * Constructor
   *
   * @param pool
   */
  state(const shared<configuration>& configuration,
        const shared<boost::mysql::connection_pool>& pool);

  /**
   * Get configuration
   *
   * @return shared<configuration>
   */
  shared<configuration> get_configuration();

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

  /**
   * Get views
   *
   * @return shared<views>
   */
  shared<views> get_views();

  /**
   * Get logger
   *
   * @return shared<logger>
   */
  shared<logger> get_logger();
};

}  // namespace copper::components

#endif