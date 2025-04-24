#ifndef COPPER_COMPONENTS_STATE_HPP
#define COPPER_COMPONENTS_STATE_HPP

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
#include <copper/components/shared.hpp>
#include <copper/components/subscriptions.hpp>
#include <mutex>

namespace copper::components {

/**
 * State
 */
class state : public shared_enabled<state> {
  /**
   * Mutex
   */
  std::mutex mutex_;

  /**
   * Websockets
   */
  uuid_hash_map_of<weaker<connection>> connections_;

  /**
   * Subscriptions
   */
  subscriptions::container subscriptions_;

 public:
  /**
   * Constructor
   */
  state();

  /**
   * Factory
   *
   * @return shared<state>
   */
  static shared<state> factory();

  /**
   * Connected
   *
   * @param connection
   */
  void connected(shared<connection>& connection);

  /**
   * Disconnected
   *
   * @param id
   */
  void disconnected(uuid id);

  /**
   * Get connections
   *
   * @return uuid_hash_map_of<weaker<connection>>
   */
  uuid_hash_map_of<weaker<connection>> get_connections() const;

  /**
   * Get subscriptions
   *
   * @return subscriptions::container
   */
  subscriptions::container get_subscriptions() const;

  /**
   * Subscribe
   *
   * @param id
   * @param channel
   */
  void subscribe(const uuid& id, const std::string& channel);

  /**
   * Unsubscribe
   *
   * @param id
   * @param channel
   */
  void unsubscribe(const uuid& id, const std::string& channel);

  /**
   * Is subscribed?
   *
   * @param id
   * @param channel
   * @return
   */
  bool is_subscribed(const uuid& id, const std::string& channel);

  /**
   * Broadcast
   *
   * @param channels
   * @param message
   */
  void broadcast(const vector_of<std::string>& channels,
                 const std::string& message);

  /**
   * Broadcast all
   *
   * @param message
   */
  void broadcast_all(const std::string& message);
};

}  // namespace copper::components

#endif
