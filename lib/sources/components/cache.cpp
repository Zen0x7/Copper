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

#include <boost/asio/bind_cancellation_slot.hpp>
#include <boost/asio/detached.hpp>
#include <copper/components/cache.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/response.hpp>
#include <copper/components/url.hpp>
#include <iostream>

namespace copper::components {
void cache::has(const std::string& key,
                const shared<boost::redis::connection>& connection,
                callback_of<int> callback) {
  LOG("[cache@has] scope_in");
  auto _request = boost::make_shared<boost::redis::request>();
  auto _response = boost::make_shared<boost::redis::response<int>>();
  _request->push("EXISTS", key);
  connection->async_exec(
      *_request, *_response,
      [callback = std::move(callback), _response, _request](
          const boost::system::error_code& error, std::size_t length) mutable {
        LOG("[cache@has] connection.async_exec scope_in");

        boost::ignore_unused(_request, length);

        callback(error ? 0 : std::get<0>(*_response).value());
        LOG("[cache@has] connection.async_exec scope_out");
      });
  LOG("[cache@has] scope_out");
}

void cache::counter_of(const std::string& key,
                       const shared<boost::redis::connection>& connection,
                       callback_of<int64_t> callback) {
  LOG("[cache@counter_of] scope_in");
  auto _request = boost::make_shared<boost::redis::request>();
  auto _response = boost::make_shared<boost::redis::response<int64_t>>();
  _request->push("GET", key);

  connection->async_exec(
      *_request, *_response,
      [callback = std::move(callback), _response, _request](
          const boost::system::error_code& error, std::size_t length) mutable {
        LOG("[cache@counter_of] connection.async_exec scope_in");

        boost::ignore_unused(_request, length);

        callback(error ? 0 : std::get<0>(*_response).value());
        LOG("[cache@counter_of] connection.async_exec scope_out");
      });
  LOG("[cache@counter_of] scope_out");
}

void cache::get_expiration_of(
    const std::string& key,
    const shared<boost::redis::connection>& connection,
    callback_of<int64_t> callback) {
  LOG("[cache@get_expiration_of] scope_in");
  auto _request = boost::make_shared<boost::redis::request>();
  auto _response = boost::make_shared<boost::redis::response<int64_t>>();
  _request->push("TTL", key);

  connection->async_exec(
      *_request, *_response,
      [callback = std::move(callback), _response, _request](
          const boost::system::error_code& error, std::size_t length) mutable {
        LOG("[cache@get_expiration_of] connection.async_exec scope_in");

        boost::ignore_unused(_request, length);

        callback(error ? 0 : std::get<0>(*_response).value());
        LOG("[cache@get_expiration_of] connection.async_exec scope_out");
      });
  LOG("[cache@get_expiration_of] scope_out");
}

void cache::increase(const std::string& key,
                     const shared<boost::redis::connection>& connection) {
  LOG("[cache@increase] scope_in");
  auto _request = boost::make_shared<boost::redis::request>();
  auto _response = boost::make_shared<boost::redis::response<std::string>>();
  _request->push("INCR", key);
  connection->async_exec(
      *_request, *_response,
      [_request, _response](const boost::system::error_code& error,
                            std::size_t length) {
        LOG("[cache@increase] connection.async_exec scope_in");

        boost::ignore_unused(_request, _response, error, length);

        LOG("[cache@increase] connection.async_exec scope_out");
      });
  LOG("[cache@increase] scope_out");
}

void cache::set(const std::string& key,
                const shared<boost::redis::connection>& connection) {
  LOG("[cache@set] scope_in");
  auto _request = boost::make_shared<boost::redis::request>();
  auto _response = boost::make_shared<boost::redis::response<std::string>>();
  _request->push("SET", key, 1, "EX", 5);
  connection->async_exec(
      *_request, *_response,
      [_request, _response](const boost::system::error_code& error,
                            std::size_t length) {
        LOG("[cache@set] connection.async_exec scope_in");
        boost::ignore_unused(_request, _response, error, length);
        LOG("[cache@set] connection.async_exec scope_out");
      });
  LOG("[cache@set] scope_out");
}

void cache::can_invoke(const req& request,
                       const std::string& ip,
                       const int max_requests,
                       const callback_of<bool, int>& callback) const {
  LOG("[cache@can_invoke] scope_in");
  const auto _url = url_from_request(request);
  std::string _key = request.method_string();
  _key.append(":");
  _key.append(_url);
  _key.append(":");
  _key.append(ip);
  get_connection([=](const shared<boost::redis::connection>& conn) mutable {
    LOG("[cache@can_invoke] get_connection scope_in");
    has(_key, conn, [=](const int exists) mutable {
      LOG("[cache@can_invoke] get_connection has scope_in");

      LOG("[cache@can_invoke] get_connection has exists: " << exists);
      if (exists != 0) {
        counter_of(_key, conn, [=](int64_t count) mutable {
          LOG("[cache@can_invoke] get_connection has counter_of scope_in");
          increase(_key, conn);
          count++;

          if (count <= max_requests) {
            callback(true, 0);
          } else {
            get_expiration_of(_key, conn, [=](const int64_t ttl) {
              LOG("[cache@can_invoke] get_connection has counter_of "
                  "get_expiration_of scope_in");
              callback(false, static_cast<int>(ttl));
              LOG("[cache@can_invoke] get_connection has counter_of "
                  "get_expiration_of scope_out");
            });
          }
          LOG("[cache@can_invoke] get_connection has counter_of scope_out");
        });
      } else {
        set(_key, conn);
        callback(true, 0);
      }
      LOG("[cache@can_invoke] get_connection has scope_out");
    });

    LOG("[cache@can_invoke] get_connection scope_out");
  });
  LOG("[cache@can_invoke] scope_out");
}

cache::cache(boost::asio::io_context& ioc) : ioc_(ioc) {
  LOG("[cache@cache] scope_in");
  redis_configuration_ = boost::make_shared<boost::redis::config>();

  const std::string _redis_port =
      std::to_string(std::stoi(dotenv::getenv("REDIS_PORT", "6379")));

  redis_configuration_->addr = boost::redis::address{
      dotenv::getenv("REDIS_HOST", "127.0.0.1"), _redis_port};

  redis_configuration_->health_check_interval = std::chrono::seconds{
      std::stoi(dotenv::getenv("REDIS_HEALTH_CHECK_INTERVAL", "60"))};

  redis_configuration_->connect_timeout = std::chrono::seconds{
      std::stoi(dotenv::getenv("REDIS_CONNECTION_TIMEOUT", "60"))};

  redis_configuration_->reconnect_wait_interval = std::chrono::seconds{
      std::stoi(dotenv::getenv("REDIS_RECONNECTION_WAIT_INTERVAL", "3"))};

  redis_configuration_->clientname =
      dotenv::getenv("REDIS_CLIENT_NAME", "copper");

  LOG("[cache@cache] scope_out");
}

void cache::get_connection(
    callback_of<const shared<boost::redis::connection>&> callback) const {
  LOG("[cache@get_connection] scope_in");
  auto _conn = boost::make_shared<boost::redis::connection>(make_strand(ioc_));
  auto self = shared_from_this();
  _conn->async_run(*redis_configuration_, {},
                   boost::asio::consign(boost::asio::detached, _conn));
  callback(_conn);
  LOG("[cache@get_connection] scope_out");
}

shared<cache> cache::factory(boost::asio::io_context& ioc) {
  return boost::make_shared<cache>(ioc);
}
}  // namespace copper::components
