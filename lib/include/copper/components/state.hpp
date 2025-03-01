#ifndef COPPER_COMPONENTS_STATE_HPP
#define COPPER_COMPONENTS_STATE_HPP

#pragma once

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