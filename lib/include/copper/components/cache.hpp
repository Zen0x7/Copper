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

class cache : public shared_enabled<cache> {
  shared<boost::redis::config> config_;

  containers::async_of<shared<boost::redis::connection>> get_connection();

 public:
  cache();

  containers::async_of<std::tuple<bool, int>> can_invoke(
      const http_request &request, const std::string &ip,
      const int &max_requests);

 private:
  static containers::async_of<int> has(
      const std::string &key,
      const shared<boost::redis::connection> &connection);

  static containers::async_of<int64_t> counter_of(
      const std::string &key,
      const shared<boost::redis::connection> &connection);

  static containers::async_of<int64_t> get_expiration_of(
      const std::string &key,
      const shared<boost::redis::connection> &connection);

  static containers::async_of<void> increase(
      const std::string &key,
      const shared<boost::redis::connection> &connection);

  static containers::async_of<void> set(
      const std::string &key,
      const shared<boost::redis::connection> &connection);

  static std::string get_key_for(const http_request &request,
                                 const std::string &ip);
};
}  // namespace copper::components

#endif