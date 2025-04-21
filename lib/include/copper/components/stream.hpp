#ifndef COPPER_MODELS_STREAM
#define COPPER_MODELS_STREAM

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
#include <copper/components/record.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>

namespace copper::components {
using namespace containers;

/**
 * Stream
 */
class stream : public shared_enabled<stream> {
 public:
  /**
   * ID
   */
  uuid id_;

  /**
   * Valid
   */
  bool valid_;

  /**
   * Data field length
   */
  std::uint32_t data_field_length_;

  /**
   * Codec ID
   */
  int codec_id_;

  /**
   * Number of data 1
   */
  std::size_t number_of_data_1_;

  /**
   * Number of data 2
   */
  std::size_t number_of_data_2_;

  /**
   * CRC
   */
  int crc_;

  /**
   * Records
   */
  vector_of<shared<record>> records_;

  /**
   * Constructor
   */
  stream()
      : id_(boost::uuids::random_generator()()),
        valid_(false),
        data_field_length_(0),
        codec_id_(0),
        number_of_data_1_(0),
        number_of_data_2_(0),
        crc_(0) {}
};

}  // namespace copper::components

#endif
