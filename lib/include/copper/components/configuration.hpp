#ifndef COPPER_COMPONENTS_CONFIGURATION_HPP
#define COPPER_COMPONENTS_CONFIGURATION_HPP

#pragma once

// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <copper/components/dotenv.hpp>
#include <copper/components/shared.hpp>
#include <mutex>

namespace copper::components {
/**
 * Configurations container
 */
class configurations : public shared_enabled<configurations> {
 public:
  /**
   * App host
   */
  std::string app_host_;

  /**
   * App debug
   */
  bool app_debug_ = false;

  /**
   * App logger
   */
  bool app_logger_ = false;

  /**
   * App key
   */
  std::string app_key_;

  /**
   * App port
   */
  int app_port_;

  /**
   * TCP port
   */
  int tcp_port_;

  /**
   * Websocket port
   */
  int websocket_port_;

  /**
   * App threads
   */
  int app_threads_;

  /**
   * App public key
   */
  std::string app_public_key_;

  /**
   * App private key
   */
  std::string app_private_key_;

  /**
   * App DH params
   */
  std::string app_dh_params_;

  /**
   * Database host
   */
  std::string database_host_;

  /**
   * Database port
   */
  int database_port_;

  /**
   * Database user
   */
  std::string database_user_;

  /**
   * Database password
   */
  std::string database_password_;

  /**
   * Database name
   */
  std::string database_name_;

  /**
   * Database thread safe
   */
  bool database_pool_thread_safe_;

  /**
   * Database pool initial size
   */
  int database_pool_initial_size_;

  /**
   * Database pool max size
   */
  int database_pool_max_size_;

  /**
   * Redis client name
   */
  std::string redis_client_name_;

  /**
   * HTTP body limit
   */
  int http_body_limit_;

  /**
   * HTTP allowed origins
   */
  std::string http_allowed_origins_;

  /**
   * Sentry DSN
   */
  std::string sentry_dsn_;

  /**
   * Sentry crashpad handler
   */
  std::string sentry_crashpad_handler_;

  /**
   * Working directory
   */
  std::string working_directory_;
};

/**
 * Configuration
 */
class configuration : public shared_enabled<configuration> {
  /**
   * Values
   */
  shared<configurations> values_;

 public:
  /**
   * Constructor
   */
  configuration();

  /**
   * Get values
   *
   * @return configurations
   */
  shared<configurations> get() { return values_; }

  /**
   * Factory
   *
   * @return shared<configuration>
   */
  static shared<configuration> factory();
};
}  // namespace copper::components

#endif
