#ifndef COPPER_COMPONENTS_EVENT_HPP
#define COPPER_COMPONENTS_EVENT_HPP

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

#include <copper/components/containers.hpp>
#include <copper/components/json.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/status_code.hpp>
#include <copper/components/uuid.hpp>

namespace copper::components {

/**
 * Event
 */
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
  vector_of<uuid> receivers_;

  /**
   * Data
   */
  json::object data_;
};
}  // namespace copper::components

#endif
