#ifndef COPPER_COMPONENTS_STATE_HPP
#define COPPER_COMPONENTS_STATE_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/containers.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/websocket.hpp>
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
  containers::uuid_hash_map_of<shared<websocket>> websockets_;

 public:
  /**
   * Constructor
   */
  state();

  /**
   * Get instance
   *
   * @return shared<state>
   */
  static shared<state> instance();

  /**
   * Connected
   *
   * @param websocket
   */
  void connected(shared<websocket>& websocket);

  /**
   * Disconnected
   *
   * @param id
   */
  void disconnected(uuid id);

  /**
   * Get websockets
   *
   * @return uuid_hash_map_of<shared<websocket>>
   */
  containers::uuid_hash_map_of<shared<websocket>> get_websockets() const;

 private:
  /**
   * Instance
   */
  static shared<state> instance_;

  /**
   * Initialization flag
   */
  static std::once_flag initialization_flag_;
};

}  // namespace copper::components

#endif