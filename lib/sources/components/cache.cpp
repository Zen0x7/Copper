#include <copper/components/cache.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/url.hpp>
#include <iostream>

namespace copper::components {

containers::async_of<int> cache::has(
    const std::string &key,
    const shared<boost::redis::connection> &connection) {
  boost::redis::request _request;
  boost::redis::response<int> _response;
  _request.push("EXISTS", key);
  co_await connection->async_exec(_request, _response, boost::asio::deferred);
  co_return std::get<0>(_response).value();
}

containers::async_of<int64_t> cache::counter_of(
    const std::string &key,
    const shared<boost::redis::connection> &connection) {
  boost::redis::request _request;
  boost::redis::response<int64_t> _response;
  _request.push("GET", key);
  co_await connection->async_exec(_request, _response, boost::asio::deferred);
  co_return std::get<0>(_response).value();
}

containers::async_of<int64_t> cache::get_expiration_of(
    const std::string &key,
    const shared<boost::redis::connection> &connection) {
  boost::redis::request _request;
  boost::redis::response<int64_t> _response;
  _request.push("TTL", key);
  co_await connection->async_exec(_request, _response, boost::asio::deferred);
  co_return std::get<0>(_response).value();
}

containers::async_of<void> cache::increase(
    const std::string &key,
    const shared<boost::redis::connection> &connection) {
  boost::redis::request _request;
  boost::redis::response<std::string> _response;
  _request.push("INCR", key);
  co_await connection->async_exec(_request, _response, boost::asio::deferred);
}

containers::async_of<void> cache::set(
    const std::string &key,
    const shared<boost::redis::connection> &connection) {
  boost::redis::request _request;
  boost::redis::response<std::string> _response;
  _request.push("SET", key, 1, "EX", 60);
  co_await connection->async_exec(_request, _response, boost::asio::deferred);
}

std::string cache::get_key_for(const request &request, const std::string &ip) {
  const auto _url = url_from_request(request);
  std::string _key = request.method_string();
  _key.append(":");
  _key.append(_url);
  _key.append(":");
  _key.append(ip);
  return _key;
}

containers::async_of<std::tuple<bool, int>> cache::can_invoke(
    const request &request, const std::string &ip,
    const int &max_requests) const {
  const auto _key = get_key_for(request, ip);
  const auto _connection = co_await this->get_connection();

  if (co_await has(_key, _connection) != 0) {
    auto _requests = co_await counter_of(_key, _connection);

    co_await increase(_key, _connection);
    _requests++;

    if (_requests <= max_requests) {
      co_return std::tuple{true, 0};
    }

    co_return std::tuple{false, co_await get_expiration_of(_key, _connection)};
  }

  co_await set(_key, _connection);
  co_return std::tuple{true, 0};
}

cache::cache()
    : redis_configuration_(boost::make_shared<boost::redis::config>()) {
  auto _configuration = configuration::instance();

  const std::string _redis_port =
      std::to_string(_configuration->get()->redis_port_);

  redis_configuration_->addr =
      boost::redis::address{_configuration->get()->redis_host_, _redis_port};

  redis_configuration_->health_check_interval =
      std::chrono::seconds{_configuration->get()->redis_health_check_interval_};

  redis_configuration_->connect_timeout =
      std::chrono::seconds{_configuration->get()->redis_connection_timeout_};

  redis_configuration_->reconnect_wait_interval = std::chrono::seconds{
      _configuration->get()->redis_reconnection_wait_interval_};

  redis_configuration_->clientname = _configuration->get()->redis_client_name_;
}

containers::async_of<shared<boost::redis::connection>> cache::get_connection()
    const {
  auto _connection = boost::make_shared<boost::redis::connection>(
      co_await boost::asio::this_coro::executor);
  _connection->async_run(*this->redis_configuration_, {},
                         consign(boost::asio::detached, _connection));
  co_return _connection;
}

containers::async_of<void> cache::publish(const std::string &channel,
                                          const std::string &data) const {
  const auto _connection = co_await this->get_connection();
  boost::redis::request _request;
  _request.push("PUBLISH", channel, data);
  co_await _connection->async_exec(_request, boost::redis::ignore,
                                   boost::asio::deferred);
}

shared<cache> cache::instance_ = nullptr;

std::once_flag cache::initialization_flag_;

shared<cache> cache::instance() {
  std::call_once(initialization_flag_,
                 [] { instance_ = boost::make_shared<cache>(); });

  return instance_->shared_from_this();
}
}  // namespace copper::components