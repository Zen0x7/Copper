#ifndef COPPER_COMPONENTS_BINARIES_HPP
#define COPPER_COMPONENTS_BINARIES_HPP

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

#include <boost/crc.hpp>
#include <cstdint>

namespace copper::components::binaries {
/**
 * Retrieve one byte of data
 *
 * @param data
 * @param from
 * @return uint8_t
 */
inline std::uint8_t one_byte(const unsigned char* data, std::size_t from = 0) {
  return data[from];
}

/**
 * Retrieve two bytes of data
 *
 * @param data
 * @param from
 * @return uint16_t
 */
inline std::uint16_t two_bytes(const unsigned char* data,
                               std::size_t from = 0) {
  const std::uint16_t value =
      *reinterpret_cast<const std::uint16_t*>(data + from);
  return __builtin_bswap16(value);
}

/**
 * Retrieve four bytes of data
 *
 * @param data
 * @param from
 * @return uint32_t
 */
inline std::uint32_t four_bytes(const unsigned char* data,
                                std::size_t from = 0) {
  const std::uint32_t value =
      *reinterpret_cast<const std::uint32_t*>(data + from);
  return __builtin_bswap32(value);
}

/**
 * Retrieve eight bytes of data
 * @param data
 * @param from
 * @return uint64_t
 */
inline std::uint64_t eight_bytes(const unsigned char* data,
                                 std::size_t from = 0) {
  const std::uint64_t value =
      *reinterpret_cast<const std::uint64_t*>(data + from);
  return __builtin_bswap64(value);
};

/**
 * Retrieve n bytes of data
 *
 * @param data
 * @param from
 * @param n
 * @return unsigned char *
 */
const unsigned char* n_bytes(const unsigned char* data,
                             std::size_t from,
                             std::size_t n);

/**
 * Get calculate CRC
 *
 * @param data
 * @param length
 * @return uint16_t
 */
inline std::uint16_t calculate_crc(const unsigned char* data,
                                   std::size_t length) {
  boost::crc_16_type crc;
  crc.process_bytes(data + 8, length);
  return crc.checksum();
}
}  // namespace copper::components::binaries

#endif
