#ifndef COPPER_COMPONENTS_STATE_HPP
#define COPPER_COMPONENTS_STATE_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/shared.hpp>
#include <mutex>

namespace copper::components {

/**
 * State
 */
class state : public shared_enabled<state> {
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