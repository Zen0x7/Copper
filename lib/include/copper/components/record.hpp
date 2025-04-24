#ifndef COPPER_COMPONENTS_RECORD_HPP
#define COPPER_COMPONENTS_RECORD_HPP

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

#include <copper/components/json.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>

namespace copper::components {

/**
 * Record
 */
class record : public shared_enabled<record> {
 public:
  /**
   * ID
   */
  uuid id_ = boost::uuids::random_generator()();

  /**
   * Timestamp
   */
  std::uint64_t timestamp_ = 0;

  /**
   * Priority
   */
  int priority_ = 0;

  /**
   * Longitude
   */
  float longitude_ = 0;

  /**
   * Latitude
   */
  float latitude_ = 0;

  /**
   * Altitude
   */
  long altitude_ = 0;

  /**
   * Angle
   */
  long angle_ = 0;

  /**
   * Satellites
   */
  int satellites_ = 0;

  /**
   * Speed
   */
  long speed_ = 0;

  /**
   * Event IO ID
   */
  long event_io_id_ = 0;

  /**
   * Number of Total ID
   */
  long number_of_total_id_ = 0;

  /**
   * Extras
   */
  json::object extras_;

  /**
   * Constructor
   */
  record() = default;
};

}  // namespace copper::components

#endif
