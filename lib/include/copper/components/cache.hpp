#pragma once

#include <copper/components/shared.hpp>
#include <copper/components/http_request.hpp>
#include <copper/components/dotenv.hpp>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/detached.hpp>
#include <boost/redis/connection.hpp>

namespace copper::components {
    class cache : public shared_enabled<cache> {
      shared<boost::redis::config> config_;

      boost::asio::awaitable<
        shared<boost::redis::connection>,
        boost::asio::strand<
          boost::asio::io_context::executor_type
        >
      > get_connection();

    public:

      cache();

      boost::asio::awaitable<
        std::tuple<bool, int>,
        boost::asio::strand<
          boost::asio::io_context::executor_type
        >
      > can_invoke(const http_request &request, const std::string &ip, const int &max_requests);

    private:

      static boost::asio::awaitable<
        int,
        boost::asio::strand<
          boost::asio::io_context::executor_type
        >
      > has(const std::string &key, const shared<boost::redis::connection> &connection);

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
      > get_expiration_of(const std::string &key, const shared<boost::redis::connection> &connection);

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

      static std::string get_key_for(const http_request &request, const std::string &ip);
    };
}