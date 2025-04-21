#ifndef COPPER_COMPONENTS_LOGGER_HPP
#define COPPER_COMPONENTS_LOGGER_HPP

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

#include <boost/mysql/error_with_diagnostics.hpp>
#include <copper/components/shared.hpp>
#include <iostream>

#ifdef DEBUG_ENABLED
#define LOG(x) std::cout << x << std::endl
#else
#define LOG(x) \
  do {         \
  } while (0)
#endif

namespace copper::components {

/**
 * Forward configuration
 */
class configuration;

/**
 * Forward state
 */
class state;

/**
 * Logger
 */
class logger : public shared_enabled<logger> {
 public:
  /**
   * Constructor
   */
  logger();

  /**
   * On database error
   *
   * @param where
   * @param error
   */
  void on_database_error(
      std::string_view where,
      const boost::mysql::error_with_diagnostics& error) const;

  /**
   * On database error
   *
   * @param where
   * @param error
   */
  void on_error(std::string_view where,
                const boost::system::error_code& error) const;

  /**
   * Factory
   *
   * @return shared<logger>
   */
  static shared<logger> factory();
};
}  // namespace copper::components

#endif
