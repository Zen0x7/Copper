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

/**
 * Database
 */
class database : public shared_enabled<database> {
  /**
   * Connections pool
   */
  shared<boost::mysql::connection_pool> pool_;

 public:
  /**
   * Constructor
   *
   * @param pool
   */
  database(const shared<boost::mysql::connection_pool> &pool);

  /**
   * Start
   */
  void start() const;

  /**
   * Retrieves user by email
   *
   * @param email
   * @return optional_of<shared<models::user>>
   */
  containers::async_of<containers::optional_of<shared<models::user>>>
  get_user_by_email(const std::string &email);

  /**
   * Retrieves user by id
   *
   * @param id
   * @return shared<models::user>
   */
  containers::async_of<shared<models::user>> get_user_by_id(uuid id);

  /**
   * Creates a session
   *
   * @param session_id
   * @param ip
   * @param port
   * @return async_of<void>
   */
  containers::async_of<void> create_session(uuid session_id,
                                            const std::string ip,
                                            uint_least16_t port);

  /**
   * Session has been closed
   *
   * @param session_id
   * @param exception
   * @return async_of<void>
   */
  containers::async_of<void> session_closed(uuid session_id,
                                            const char exception[]);

  /**
   * Session is encrypted
   *
   * @param session_id
   * @return async_of<void>
   */
  containers::async_of<void> session_is_encrypted(uuid session_id);

  /**
   * Session is upgraded
   *
   * @param session_id
   * @return async_of<void>
   */
  containers::async_of<void> session_is_upgrade(uuid session_id);

  /**
   * Creates a invocation
   *
   * @param request
   * @param response
   * @return async_of<void>
   */
  containers::async_of<void> create_invocation(
      shared<models::request> request, shared<models::response> response);
};

}  // namespace copper::components

#endif