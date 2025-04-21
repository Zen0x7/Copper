#ifndef COPPER_COMPONENTS_DATABASE_HPP
#define COPPER_COMPONENTS_DATABASE_HPP

#pragma once

// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <boost/mysql/connection_pool.hpp>
#include <copper/components/connection.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/request.hpp>
#include <copper/components/response.hpp>
#include <copper/components/tracker.hpp>
#include <copper/components/user.hpp>

namespace copper::components {
using namespace containers;

/**
 * Forward core
 */
class core;

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
   */
  database() = default;

  /**
   * Stop
   */
  void stop() const;

  /**
   * Retrieves user by email
   *
   * @param core
   * @param email
   * @param callback
   * @return void
   */
  void get_user_by_email(const shared<core>& core,
                         const std::string& email,
                         callback_of<optional_of<shared<user>>> callback) const;

  /**
   * Retrieves user by id
   *
   * @param core
   * @param id
   * @param callback
   * @return void
   */
  void get_user_by_id(const shared<core>& core,
                      const uuid& id,
                      callback_of<optional_of<shared<user>>> callback) const;

  /**
   * Creates a connection
   *
   * @param core
   * @param connection
   * @return void
   */
  void create_connection(const shared<core>& core,
                         const shared<connection>& connection) const;

  /**
   * Session has been closed
   *
   * @param core
   * @param connection_id
   * @param exception
   * @return void
   */
  void connection_closed(const shared<core>& core,
                         uuid connection_id,
                         const char exception[]) const;

  /**
   * Session is upgraded
   *
   * @param core
   * @param connection_id
   * @return void
   */
  void connection_is_websocket(const shared<core>& core,
                               uuid connection_id) const;

  /**
   * Create invocation
   *
   * @param core
   * @param request
   * @param response
   * @return void
   */
  void create_invocation(const shared<core>& core,
                         const shared<request>& request,
                         const shared<response>& response) const;

  /**
   * Find tracker
   *
   * @param core
   * @param imei
   * @param callback
   * @return void
   */
  void find_tracker(const shared<core>& core,
                    const std::string& imei,
                    callback_of<optional_of<shared<tracker>>> callback) const;

  /**
   * Create user
   *
   * @param core
   * @param name
   * @param email
   * @param password
   * @param callback
   * @return async_of<void>
   */
  void create_user(const boost::shared_ptr<core>& core,
                   const std::string& name,
                   const std::string& email,
                   const std::string& password,
                   callback_of<bool> callback) const;

  /**
   * Factory
   *
   * @return shared<database>
   */
  static shared<database> factory();

  /**
   * Initialize
   *
   * @param ioc
   * @param core
   * @return void
   */
  void init(boost::asio::io_context& ioc, const shared<core>& core);
};

}  // namespace copper::components

#endif
