#ifndef COPPER_COMPONENTS_DATABASE_HPP
#define COPPER_COMPONENTS_DATABASE_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/mysql/connection_pool.hpp>
#include <copper/components/containers.hpp>
#include <copper/models/request.hpp>
#include <copper/models/response.hpp>
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
  explicit database(const shared<boost::mysql::connection_pool> &pool);

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
  get_user_by_email(std::string email);

  /**
   * Retrieves user by id
   *
   * @param id
   * @return shared<models::user>
   */
  containers::async_of<containers::optional_of<shared<models::user>>>
  get_user_by_id(uuid id);

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
   * Session is upgraded
   *
   * @param session_id
   * @return async_of<void>
   */
  containers::async_of<void> session_is_upgrade(uuid session_id);

  /**
   * Create invocation
   *
   * @param request
   * @param response
   * @return async_of<void>
   */
  containers::async_of<void> create_invocation(
      shared<models::request> request, shared<models::response> response);

  /**
   * Get instance
   *
   * @return shared<cache>
   */
  static shared<database> instance();

  /**
   * Instance
   */
  static shared<database> instance_;

  /**
   * Setup
   */
  static void setup(boost::asio::io_context &context);
};

}  // namespace copper::components

#endif