#ifndef COPPER_COMPONENTS_CACHE_HPP
#define COPPER_COMPONENTS_CACHE_HPP

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

#include <boost/redis/connection.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/req.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {
using namespace containers;

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
  shared<boost::redis::config> redis_configuration_ =
      boost::make_shared<boost::redis::config>();

  /**
   * IO Context
   */
  boost::asio::io_context& ioc_;

 public:
  /**
   * Constructor
   */
  cache(boost::asio::io_context& ioc);

  /**
   * Retrieve connection
   *
   * @param callback
   * @return void
   */
  void get_connection(
      callback_of<const shared<boost::redis::connection>&> callback) const;

  /**
   * Determines if request can be invoked
   *
   * @param request
   * @param ip
   * @param max_requests
   * @param callback
   * @return void
   */
  void can_invoke(const req& request,
                  const std::string& ip,
                  int max_requests,
                  const callback_of<bool, int>& callback) const;

  /**
   * Factory
   *
   * @return shared<cache>
   */
  static shared<cache> factory(boost::asio::io_context& ioc);

 private:
  /**
   * Determines if cache has key
   *
   * @param key
   * @param connection
   * @param callback
   * @return void
   */
  static void has(const std::string& key,
                  const shared<boost::redis::connection>& connection,
                  callback_of<int> callback);

  /**
   * Retrieves key's invoked times
   *
   * @param key
   * @param connection
   * @param callback
   * @return void
   */
  static void counter_of(const std::string& key,
                         const shared<boost::redis::connection>& connection,
                         callback_of<int64_t> callback);

  /**
   * Retrieve key's expiration at
   *
   * @param key
   * @param connection
   * @param callback
   * @return void
   */
  static void get_expiration_of(
      const std::string& key,
      const shared<boost::redis::connection>& connection,
      callback_of<int64_t> callback);

  /**
   * Increase key
   *
   * @param key
   * @param connection
   * @return void
   */
  static void increase(const std::string& key,
                       const shared<boost::redis::connection>& connection);

  /**
   * Set key
   *
   * @param key
   * @param connection
   * @return void
   */
  static void set(const std::string& key,
                  const shared<boost::redis::connection>& connection);
};
}  // namespace copper::components

#endif
