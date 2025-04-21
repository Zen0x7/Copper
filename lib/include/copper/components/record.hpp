#ifndef COPPER_MODELS_RECORD_HPP
#define COPPER_MODELS_RECORD_HPP

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
  std::uint64_t timestamp_;

  /**
   * Priority
   */
  int priority_;

  /**
   * Longitude
   */
  float longitude_;

  /**
   * Latitude
   */
  float latitude_;

  /**
   * Altitude
   */
  long altitude_;

  /**
   * Angle
   */
  long angle_;

  /**
   * Satellites
   */
  int satellites_;

  /**
   * Speed
   */
  long speed_;

  /**
   * Event IO ID
   */
  long event_io_id_;

  /**
   * Number of Total ID
   */
  long number_of_total_id_;

  /**
   * Extras
   */
  json::object extras_;

  /**
   * Constructor
   */
  record()
      : timestamp_(0),
        priority_(0),
        longitude_(0),
        latitude_(0),
        altitude_(0),
        angle_(0),
        satellites_(0),
        speed_(0),
        event_io_id_(0),
        number_of_total_id_(0) {}
};

}  // namespace copper::components

#endif
