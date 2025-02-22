#ifndef COPPER_COMPONENTS_CONFIGURATION_HPP
#define COPPER_COMPONENTS_CONFIGURATION_HPP

#include <copper/components/dotenv.hpp>
#include <copper/components/shared.hpp>

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
   * App key
   */
  std::string app_key_;

  /**
   * App port
   */
  int app_port_;

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
   * Redis host
   */
  std::string redis_host_;

  /**
   * Redis port
   */
  int redis_port_;

  /**
   * Redis health check interval
   */
  int redis_health_check_interval_;

  /**
   * Redis connection timeout
   */
  int redis_connection_timeout_;

  /**
   * Redis reconnection wait interval
   */
  int redis_reconnection_wait_interval_;

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
  shared<configurations> get();
};
}  // namespace copper::components

#endif
