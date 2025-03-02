#ifndef COPPER_COMPONENTS_CACHE_HPP
#define COPPER_COMPONENTS_CACHE_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/redis/connection.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/request.hpp>
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
  /**
   * Configuration
   */
  shared<boost::redis::config> redis_configuration_;

 public:
  /**
   * Constructor
   */
  cache();

  /**
   * Retrieve connection
   *
   * @return shared<redis::connection>
   */
  containers::async_of<shared<boost::redis::connection>> get_connection() const;

  /**
   * Determines if request can be invoked
   *
   * @param request
   * @param ip
   * @param max_requests
   * @return async_of<tuple_of<bool, int>>
   */
  containers::async_of<containers::tuple_of<bool, int>> can_invoke(
      request request, std::string ip, int max_requests) const;

  /**
   * Publish
   *
   * @param channel
   * @param data
   * @return async_of<void>
   */
  containers::async_of<void> publish(const std::string &channel,
                                     const std::string &data) const;

  /**
   * Get instance
   *
   * @return shared<cache>
   */
  static shared<cache> instance();

 private:
  /**
   * Instance
   */
  static shared<cache> instance_;

  /**
   * Initialization flag
   */
  static std::once_flag initialization_flag_;

  /**
   * Determines if cache has key
   *
   * @param key
   * @param connection
   * @return async_of<int>
   */
  static containers::async_of<int> has(
      std::string key, shared<boost::redis::connection> connection);

  /**
   * Retrieves key's invoked times
   *
   * @param key
   * @param connection
   * @return async_of<int64_t>
   */
  static containers::async_of<int64_t> counter_of(
      std::string key, shared<boost::redis::connection> connection);

  /**
   * Retrieve key's expiration at
   *
   * @param key
   * @param connection
   * @return async_of<void>
   */
  static containers::async_of<int64_t> get_expiration_of(
      std::string key, shared<boost::redis::connection> connection);

  /**
   * Increase key
   *
   * @param key
   * @param connection
   * @return async_of<void>
   */
  static containers::async_of<void> increase(
      std::string key, shared<boost::redis::connection> connection);

  /**
   * Set key
   *
   * @param key
   * @param connection
   * @return async_of<void>
   */
  static containers::async_of<void> set(
      std::string key, shared<boost::redis::connection> connection);

  /**
   * Generate request based key
   *
   * @param request
   * @param ip
   * @return async_of<void>
   */
  static std::string get_key_for(const request &request,
                                 const std::string_view &ip);
};
}  // namespace copper::components

#endif