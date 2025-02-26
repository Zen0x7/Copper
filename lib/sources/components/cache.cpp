#include <copper/components/cache.hpp>
#include <copper/components/configuration.hpp>
#include <iostream>

namespace copper::components {

containers::async_of<int> cache::has(
    const std::string &key,
    const shared<boost::redis::connection> &connection) {
  boost::redis::request request;
  boost::redis::response<int> response;
  request.push("EXISTS", key);
  co_await connection->async_exec(request, response, boost::asio::deferred);
  co_return std::get<0>(response).value();
}

containers::async_of<int64_t> cache::counter_of(
    const std::string &key,
    const shared<boost::redis::connection> &connection) {
  boost::redis::request request;
  boost::redis::response<int64_t> response;
  request.push("GET", key);
  co_await connection->async_exec(request, response, boost::asio::deferred);
  co_return std::get<0>(response).value();
}

containers::async_of<int64_t> cache::get_expiration_of(
    const std::string &key,
    const shared<boost::redis::connection> &connection) {
  boost::redis::request request;
  boost::redis::response<int64_t> response;
  request.push("TTL", key);
  co_await connection->async_exec(request, response, boost::asio::deferred);
  co_return std::get<0>(response).value();
}

containers::async_of<void> cache::increase(
    const std::string &key,
    const shared<boost::redis::connection> &connection) {
  boost::redis::request request;
  boost::redis::response<std::string> response;
  request.push("INCR", key);
  co_await connection->async_exec(request, response, boost::asio::deferred);
}

containers::async_of<void> cache::set(
    const std::string &key,
    const shared<boost::redis::connection> &connection) {
  boost::redis::request request;
  boost::redis::response<std::string> response;
  request.push("SET", key, 1, "EX", 60);
  co_await connection->async_exec(request, response, boost::asio::deferred);
}

std::string cache::get_key_for(const request &request, const std::string &ip) {
  const size_t query_ask_mark_position = request.target().find('?');
  const bool path_has_params = query_ask_mark_position != std::string::npos;
  const std::string path{
      path_has_params ? request.target().substr(0, query_ask_mark_position)
                      : request.target()};
  std::string key = request.method_string();
  key.append(":");
  key.append(path);
  key.append(":");
  key.append(ip);
  return key;
}

containers::async_of<std::tuple<bool, int>> cache::can_invoke(
    const request &request, const std::string &ip, const int &max_requests) {
  const auto key = get_key_for(request, ip);
  auto connection = co_await this->get_connection();

  if (co_await has(key, connection) != 0) {
    auto requests = co_await counter_of(key, connection);

    co_await increase(key, connection);
    requests++;

    if (requests <= max_requests) {
      co_return std::tuple{true, 0};
    }

    co_return std::tuple{false, co_await get_expiration_of(key, connection)};
  }

  co_await set(key, connection);
  co_return std::tuple{true, 0};
}

cache::cache(const shared<configuration> &configuration)
    : configuration_(configuration),
      redis_configuration_(boost::make_shared<boost::redis::config>()) {
  std::string _redis_port = std::to_string(configuration_->get()->redis_port_);

  redis_configuration_->addr =
      boost::redis::address{configuration_->get()->redis_host_, _redis_port};

  redis_configuration_->health_check_interval =
      std::chrono::seconds{configuration_->get()->redis_health_check_interval_};

  redis_configuration_->connect_timeout =
      std::chrono::seconds{configuration_->get()->redis_connection_timeout_};

  redis_configuration_->reconnect_wait_interval = std::chrono::seconds{
      configuration_->get()->redis_reconnection_wait_interval_};

  redis_configuration_->clientname = configuration_->get()->redis_client_name_;
}

containers::async_of<shared<boost::redis::connection>> cache::get_connection() {
  auto conn = boost::make_shared<boost::redis::connection>(
      co_await boost::asio::this_coro::executor);
  conn->async_run(*this->redis_configuration_, {},
                  consign(boost::asio::detached, conn));
  co_return conn;
}

containers::async_of<void> cache::publish(const std::string &channel,
                                          const std::string &data) {
  auto connection = co_await this->get_connection();
  boost::redis::request request;
  request.push("PUBLISH", channel, data);
  co_await connection->async_exec(request, boost::redis::ignore,
                                  boost::asio::deferred);
}
}  // namespace copper::components