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
   * Router
   */
  shared<router> router_;

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
   * Get Router
   *
   * @return shared<router>
   */
  shared<router> get_router();

  /**
   * Get database
   *
   * @return shared<database>
   */
  shared<database> get_database();
};

}  // namespace copper::components

#endif