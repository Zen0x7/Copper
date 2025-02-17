#pragma once

#include <copper/components/shared.hpp>
#include <copper/components/http_request.hpp>
#include <copper/components/dotenv.hpp>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/detached.hpp>
#include <boost/redis/connection.hpp>

namespace copper::components {
    class redis_service : public shared_enabled<redis_service> {
      shared<boost::redis::config> redis_config_;

      boost::asio::awaitable<
        shared<boost::redis::connection>,
        boost::asio::strand<
          boost::asio::io_context::executor_type
        >
      > get_redis_connection();

    public:

      redis_service();

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