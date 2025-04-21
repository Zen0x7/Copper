#ifndef COPPER_PROTOCOL_HPP
#define COPPER_PROTOCOL_HPP

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
#include <copper/components/containers.hpp>
#include <copper/components/core.hpp>
#include <copper/components/json.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {
using namespace containers;

/**
 * Protocol response
 */
class protocol_response : public shared_enabled<protocol_response> {
 public:
  /**
   * Should reply
   */
  bool should_reply_ = false;

  /**
   * Receivers
   */
  std::vector<uuid> receivers_;

  /**
   * Data
   */
  json::object data_ = {};
};

/**
 * Protocol handler from websocket
 *
 * @param core
 * @param server_id
 * @param connection
 * @param buffer
 * @return shared<protocol_response>
 */
shared<protocol_response> protocol_handler_from_websocket(
    const shared<core>& core,
    uuid server_id,
    const shared<connection>& connection,
    boost::beast::flat_buffer& buffer);

/**
 * Protocol handler any
 *
 * @param core
 * @param server_id
 * @param connection_id
 * @param id
 * @param action
 * @param value
 * @param response
 * @param from_websockets
 * @return shared<protocol_response>
 */
shared<protocol_response> protocol_handler_any(
    const shared<core>& core,
    uuid server_id,
    uuid connection_id,
    json::string id,
    const json::string& action,
    json::value value,
    shared<protocol_response> response,
    bool from_websockets);
}  // namespace copper::components

#endif  // COPPER_PROTOCOL_HPP
