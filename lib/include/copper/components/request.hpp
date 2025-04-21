#ifndef COPPER_COMPONENTS_REQUEST_HPP
#define COPPER_COMPONENTS_REQUEST_HPP

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

#include <copper/components/connection.hpp>
#include <copper/components/header.hpp>
#include <copper/components/query.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>
#include <utility>

namespace copper::components {

/**
 * Request
 */
class request : public shared_enabled<request> {
 public:
  /**
   * ID
   */
  uuid id_ = boost::uuids::random_generator()();

  /**
   * Connection
   */
  shared<connection> connection_;

  /**
   * Version
   */
  std::string version_;

  /**
   * Method
   */
  std::string method_;

  /**
   * Path
   */
  std::string path_;

  /**
   * Query
   */
  std::string query_;

  /**
   * Headers
   */
  std::string headers_;

  /**
   * Body
   */
  std::string body_;

  /**
   * Started at
   */
  long started_at_;

  /**
   * Finished at
   */
  long finished_at_;

  /**
   * Duration
   */
  long duration_;

  /**
   * Constructor
   *
   * @param connection
   * @param version
   * @param method
   * @param path
   * @param query
   * @param headers
   * @param body
   * @param started_at
   * @param finished_at
   * @param duration
   */
  request(const shared<connection>& connection,
          std::string version,
          std::string method,
          std::string path,
          std::string query,
          std::string headers,
          std::string body,
          long started_at,
          long finished_at,
          long duration)
      : connection_(connection),
        version_(std::move(version)),
        method_(std::move(method)),
        path_(std::move(path)),
        query_(std::move(query)),
        headers_(std::move(headers)),
        body_(std::move(body)),
        started_at_(started_at),
        finished_at_(finished_at),
        duration_(duration) {}
};

/**
 * Create request from HTTP request
 *
 * @param connection
 * @param now
 * @param req
 * @return
 */
shared<request> request_from_req(const shared<connection>& connection,
                                 long now,
                                 const req& req);

}  // namespace copper::components

#endif
