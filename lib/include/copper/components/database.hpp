#ifndef COPPER_COMPONENTS_DATABASE_HPP
#define COPPER_COMPONENTS_DATABASE_HPP

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

  containers::async_of<containers::optional_of<shared<copper::models::user>>>
  get_user_by_email(const std::string &email);

  containers::async_of<shared<copper::models::user>> get_user_by_id(uuid id);

  containers::async_of<void> create_session(uuid session_id,
                                            const std::string ip,
                                            uint_least16_t port);

  containers::async_of<void> session_closed(uuid session_id,
                                            const char exception[]);

  containers::async_of<void> session_is_encrypted(uuid session_id);

  containers::async_of<void> session_is_upgrade(uuid session_id);

  containers::async_of<void> create_request(
      shared<copper::models::request> request,
      shared<copper::models::response> response);
};

}  // namespace copper::components

#endif