#include <copper/components/cache.hpp>

namespace copper::components {
    boost::asio::awaitable<int, boost::asio::strand<boost::asio::io_context::executor_type>>
    cache::has(const std::string &key, const shared<boost::redis::connection> & connection) {
      boost::redis::request request;
      boost::redis::response<int> response;
      request.push("EXISTS", key);
      co_await connection->async_exec(request, response, boost::asio::deferred);
      co_return std::get<0>(response).value();
    }

    boost::asio::awaitable<int64_t, boost::asio::strand<boost::asio::io_context::executor_type>>
    cache::counter_of(const std::string &key, const shared<boost::redis::connection> & connection) {
      boost::redis::request request;
      boost::redis::response<int64_t> response;
      request.push("GET", key);
      co_await connection->async_exec(request, response, boost::asio::deferred);
      co_return std::get<0>(response).value();
    }

    boost::asio::awaitable<int64_t, boost::asio::strand<boost::asio::io_context::executor_type>>
    cache::get_expiration_of(const std::string &key, const shared<boost::redis::connection> & connection) {
      boost::redis::request request;
      boost::redis::response<int64_t> response;
      request.push("TTL", key);
      co_await connection->async_exec(request, response, boost::asio::deferred);
      co_return std::get<0>(response).value();
    }

    boost::asio::awaitable<void, boost::asio::strand<boost::asio::io_context::executor_type>>
    cache::increase(const std::string &key, const shared<boost::redis::connection> & connection) {
      boost::redis::request request;
      boost::redis::response<std::string> response;
      request.push("INCR", key);
      co_await connection->async_exec(request, response, boost::asio::deferred);
    }

    boost::asio::awaitable<void, boost::asio::strand<boost::asio::io_context::executor_type>>
    cache::set(const std::string &key, const shared<boost::redis::connection> & connection) {
      boost::redis::request request;
      boost::redis::response<std::string> response;
      request.push("SET", key, 1, "EX", 60);
      co_await connection->async_exec(request, response, boost::asio::deferred);
    }

    std::string cache::get_key_for(const http_request &request, const std::string &ip) {
      const size_t query_ask_mark_position = request.target().find('?');
      const bool path_has_params = query_ask_mark_position != std::string::npos;
      const std::string path{path_has_params ? request.target().substr(0, query_ask_mark_position) : request.target()};
      std::string key = request.method_string();
      key.append(":");
      key.append(path);
      key.append(":");
      key.append(ip);
      return key;
    }

    boost::asio::awaitable<
      std::tuple<bool, int>,
      boost::asio::strand<
        boost::asio::io_context::executor_type
      >
    > cache::can_invoke(const http_request &request, const std::string &ip, const int &max_requests) {
      const auto key = get_key_for(request, ip);
      auto connection = co_await this->get_connection();

      if (co_await has(key, connection) != 0) {

        const auto requests = co_await counter_of(key, connection);

        co_await increase(key, connection);

        if (requests >= max_requests) {
          co_return std::tuple{false, co_await get_expiration_of(key, connection)};
        }

        co_return std::tuple{true, 0};
      }

      co_await set(key, connection);
      co_return std::tuple{true, 0};
    }

    cache::cache() : config_(boost::make_shared<boost::redis::config>()) {

      config_->addr = boost::redis::address{
        dotenv::getenv("REDIS_HOST", "127.0.0.1"),
        dotenv::getenv("REDIS_PORT", "6379")
      };

      config_->health_check_interval = std::chrono::seconds{
        std::stoi(dotenv::getenv("REDIS_HEALTH_CHECK_INTERVAL", "60")),
      };

      config_->connect_timeout = std::chrono::seconds{
        std::stoi(dotenv::getenv("REDIS_CONNECTION_TIMEOUT", "60")),
      };

      config_->reconnect_wait_interval = std::chrono::seconds{
        std::stoi(dotenv::getenv("REDIS_RECONNECTION_WAIT_INTERVAL", "5")),
      };

      config_->clientname = dotenv::getenv("REDIS_CLIENT_NAME", "Copper");
    }

    boost::asio::awaitable<shared<boost::redis::connection>, boost::asio::strand<boost::asio::io_context::executor_type>>
    cache::get_connection() {
      auto conn = boost::make_shared<boost::redis::connection>(co_await boost::asio::this_coro::executor);
      conn->async_run(*this->config_, {}, boost::asio::consign(boost::asio::detached, conn));
      co_return conn;
    }
}