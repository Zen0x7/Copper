#ifndef COPPER_COMPONENTS_CACHE_HPP
#define COPPER_COMPONENTS_CACHE_HPP

#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/strand.hpp>
#include <boost/redis/connection.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/dotenv.hpp>
#include <copper/components/http_request.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {
/**
 * Forward configuration
 */
class configuration;

/**
 * Cache
 */
class cache : public shared_enabled<cache> {
  shared<configuration> configuration_;

  /**
   * Configuration
   */
  shared<boost::redis::config> redis_configuration_;

 public:
  /**
   * Constructor
   * @param configuration
   */
  cache(const shared<configuration> &configuration);

  /**
   * Retrieve connection
   *
   * @return shared<redis::connection>
   */
  containers::async_of<shared<boost::redis::connection>> get_connection();

  /**
   * Determines if request can be invoked
   *
   * @param request
   * @param ip
   * @param max_requests
   * @return async_of<tuple_of<bool, int>>
   */
  containers::async_of<containers::tuple_of<bool, int>> can_invoke(
      const http_request &request, const std::string &ip,
      const int &max_requests);

 private:
  /**
   * Determines if cache has key
   *
   * @param key
   * @param connection
   * @return async_of<int>
   */
  static containers::async_of<int> has(
      const std::string &key,
      const shared<boost::redis::connection> &connection);

  /**
   * Retrieves key's invoked times
   *
   * @param key
   * @param connection
   * @return async_of<int64_t>
   */
  static containers::async_of<int64_t> counter_of(
      const std::string &key,
      const shared<boost::redis::connection> &connection);

  /**
   * Retrieve key's expiration at
   *
   * @param key
   * @param connection
   * @return async_of<void>
   */
  static containers::async_of<int64_t> get_expiration_of(
      const std::string &key,
      const shared<boost::redis::connection> &connection);

  /**
   * Increase key
   *
   * @param key
   * @param connection
   * @return async_of<void>
   */
  static containers::async_of<void> increase(
      const std::string &key,
      const shared<boost::redis::connection> &connection);

  /**
   * Set key
   *
   * @param key
   * @param connection
   * @return async_of<void>
   */
  static containers::async_of<void> set(
      const std::string &key,
      const shared<boost::redis::connection> &connection);

  /**
   * Publish
   *
   * @param channel
   * @param data
   * @param connection
   * @return async_of<void>
   */
  containers::async_of<void> publish(const std::string &channel,
                                     const std::string &data);

  /**
   * Generate request based key
   *
   * @param request
   * @param ip
   * @return async_of<void>
   */
  static std::string get_key_for(const http_request &request,
                                 const std::string &ip);
};
}  // namespace copper::components

#endif