#ifndef COPPER_COMPONENTS_EVENT_HPP
#define COPPER_COMPONENTS_EVENT_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/containers.hpp>
#include <copper/components/json.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/status_code.hpp>
#include <copper/components/uuid.hpp>

namespace copper::components {
class event : public shared_enabled<event> {
 public:
  /**
   * ID
   */
  uuid id_;

  /**
   * Status Code
   */
  status_code status_code_;

  /**
   * Should respond
   */
  bool should_respond_ = false;

  /**
   * Receivers
   */
  containers::vector_of<uuid> receivers_;

  /**
   * Data
   */
  json::object data_;
};
}  // namespace copper::components

#endif
