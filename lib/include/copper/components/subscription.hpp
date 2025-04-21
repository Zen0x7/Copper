#ifndef COPPER_COMPONENTS_SUBSCRIPTION_HPP
#define COPPER_COMPONENTS_SUBSCRIPTION_HPP

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

#include <copper/components/uuid.hpp>

namespace copper::components {

/**
 * Subscription
 */
struct subscription {
  /**
   * Connection ID
   */
  uuid connection_id_;

  /**
   * Channel
   */
  std::string channel_;

  /**
   * Constructor
   *
   * @param connection_id
   * @param channel_name
   */
  subscription(const uuid connection_id, std::string channel_name)
      : connection_id_(connection_id), channel_(std::move(channel_name)) {}
};
}  // namespace copper::components

#endif  // COPPER_COMPONENTS_SUBSCRIPTION_HPP
