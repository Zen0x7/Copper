#ifndef COPPER_MODELS_RESPONSE_HPP
#define COPPER_MODELS_RESPONSE_HPP

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

#include <copper/components/request.hpp>
#include <copper/components/res.hpp>
#include <copper/components/shared.hpp>
#include <utility>

namespace copper::components {
/**
 * Response
 */
class response : public shared_enabled<response> {
 public:
  /**
   * ID
   */
  uuid id_ = boost::uuids::random_generator()();

  /**
   * Session ID
   */
  uuid connection_id_;

  /**
   * Request ID
   */
  uuid request_id_;

  /**
   * Status code
   */
  int status_code_;

  /**
   * Headers
   */
  std::string headers_;

  /**
   * Body
   */
  std::string body_;

  /**
   * Protected
   */
  bool protected_ = false;

  /**
   * Compressed
   */
  bool compressed_ = false;

  /**
   * Constructor
   *
   * @param connection_id
   * @param request_id
   * @param status_code
   * @param headers
   * @param body
   * @param compressed
   */
  response(const uuid connection_id,
           const uuid request_id,
           const int status_code,
           std::string headers,
           std::string body,
           const bool compressed)
      : connection_id_(connection_id),
        request_id_(request_id),
        status_code_(status_code),
        headers_(std::move(headers)),
        body_(std::move(body)),
        compressed_(compressed) {}
};

/**
 * Create response from HTTP response
 *
 * @param connection
 * @param request
 * @param res
 * @return
 */
shared<response> response_factory(const shared<connection>& connection,
                                  const shared<request>& request,
                                  const res& res);
}  // namespace copper::components

#endif
