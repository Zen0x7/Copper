#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/mysql/connection_pool.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/dotenv.hpp>
#include <copper/components/report.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>
#include <copper/models/request.hpp>
#include <copper/models/response.hpp>
#include <copper/models/session.hpp>
#include <copper/models/user.hpp>

namespace copper::components {
class database : public shared_enabled<database> {
  shared<boost::mysql::connection_pool> pool_;

 public:
  database(const shared<boost::mysql::connection_pool> &pool);

  void start();

  boost::asio::awaitable<
      containers::optional_of<shared<copper::models::user>>,
      boost::asio::strand<boost::asio::io_context::executor_type>>
  get_user_by_email(const std::string &email);

  boost::asio::awaitable<
      shared<copper::models::user>,
      boost::asio::strand<boost::asio::io_context::executor_type>>
  get_user_by_id(uuid id);

  boost::asio::awaitable<
      void, boost::asio::strand<boost::asio::io_context::executor_type>>
  create_session(uuid session_id, const std::string ip, uint_least16_t port);

  boost::asio::awaitable<
      void, boost::asio::strand<boost::asio::io_context::executor_type>>
  session_closed(uuid session_id, const char exception[]);

  boost::asio::awaitable<
      void, boost::asio::strand<boost::asio::io_context::executor_type>>
  session_is_encrypted(uuid session_id);

  boost::asio::awaitable<
      void, boost::asio::strand<boost::asio::io_context::executor_type>>
  session_is_upgrade(uuid session_id);

  boost::asio::awaitable<
      void, boost::asio::strand<boost::asio::io_context::executor_type>>
  create_request(shared<copper::models::request> request,
                 shared<copper::models::response> response);
};
}  // namespace copper::components