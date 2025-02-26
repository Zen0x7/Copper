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
 * Forward Router
 */
class router;

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
   * Configuration
   */
  shared<configuration> configuration_;

  /**
   * Router
   */
  shared<router> router_;

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
   * @param configuration
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
   * Get Router
   *
   * @return shared<router>
   */
  shared<router> get_router();

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