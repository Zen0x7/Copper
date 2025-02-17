#pragma once

#include <copper/components/shared.hpp>
#include <copper/components/http_request.hpp>
#include <copper/components/dotenv.hpp>

#include <boost/asio.hpp>
#include <boost/redis.hpp>
#include <boost/redis/connection.hpp>

namespace copper::components {
    class redis_service : public shared_enabled<redis_service> {
      shared<boost::redis::config> redis_config_;

      boost::asio::awaitable<
        shared<boost::redis::connection>,
        boost::asio::strand<
          boost::asio::io_context::executor_type
        >
      > get_redis_connection() {
        auto conn = boost::make_shared<boost::redis::connection>(co_await boost::asio::this_coro::executor);
        conn->async_run(*this->redis_config_, {}, boost::asio::consign(boost::asio::detached, conn));
        co_return conn;
      }

    public:

      redis_service() : redis_config_(boost::make_shared<boost::redis::config>()) {

        redis_config_->addr = boost::redis::address{
          dotenv::getenv("REDIS_HOST", "127.0.0.1"),
          dotenv::getenv("REDIS_PORT", "6379")
        };

        redis_config_->health_check_interval = std::chrono::seconds{
          std::stoi(dotenv::getenv("REDIS_HEALTH_CHECK_INTERVAL", "60")),
        };

        redis_config_->connect_timeout = std::chrono::seconds{
          std::stoi(dotenv::getenv("REDIS_CONNECTION_TIMEOUT", "60")),
        };

        redis_config_->reconnect_wait_interval = std::chrono::seconds{
          std::stoi(dotenv::getenv("REDIS_RECONNECTION_WAIT_INTERVAL", "5")),
        };

        redis_config_->clientname = dotenv::getenv("REDIS_CLIENT_NAME", "Copper");
      }

      boost::asio::awaitable<
        std::tuple<bool, int>,
        boost::asio::strand<
          boost::asio::io_context::executor_type
        >
      > is_alive(const http_request &request, const std::string &ip, const int &max_requests);

    private:

      static boost::asio::awaitable<
        int,
        boost::asio::strand<
          boost::asio::io_context::executor_type
        >
      > exists(const std::string &key, const shared<boost::redis::connection> &connection);

      static boost::asio::awaitable<
        int64_t,
        boost::asio::strand<
          boost::asio::io_context::executor_type
        >
      > counter_of(const std::string &key, const shared<boost::redis::connection> &connection);

      static boost::asio::awaitable<
        int64_t,
        boost::asio::strand<
          boost::asio::io_context::executor_type
        >
      > ttl_of(const std::string &key, const shared<boost::redis::connection> &connection);

      static boost::asio::awaitable<
        void,
        boost::asio::strand<
          boost::asio::io_context::executor_type
        >
      > increase(const std::string &key, const shared<boost::redis::connection> &connection);


      static boost::asio::awaitable<
        void,
        boost::asio::strand<
          boost::asio::io_context::executor_type
        >
      > set(const std::string &key, const shared<boost::redis::connection> &connection);

      static std::string get_key_of(const http_request &request, const std::string &ip);
    };
}