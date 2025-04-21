#ifndef COPPER_COMPONENTS_REPORT_HPP
#define COPPER_COMPONENTS_REPORT_HPP

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

#include <boost/beast/core/error.hpp>

namespace copper::components {

// LCOV_EXCL_START
/**
 * Report exception
 */
class report_exception final : public std::runtime_error {
 public:
  /**
   * Constructor
   * @param what
   */
  explicit report_exception(const std::string& what = "")
      : std::runtime_error(what) {}
};
// LCOV_EXCL_STOP

/**
 * Report
 *
 * @param ec
 * @param what
 */
void report(const boost::beast::error_code& ec, char const* what);

/**
 * Report for OpenSSL
 */
[[noreturn]] void report_for_openssl();

}  // namespace copper::components

#endif
