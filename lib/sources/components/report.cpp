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

#include <openssl/err.h>

#include <boost/stacktrace.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/report.hpp>
#include <iostream>

namespace copper::components {

void report(const boost::beast::error_code& ec, const char* what) {
  LOG(boost::stacktrace::stacktrace());
  LOG(what << ": " << ec.message());
}

// LCOV_EXCL_START
void report_for_openssl() {
  const unsigned long _error_code = ERR_get_error();
  std::string _error_message(256, '\0');
  ERR_error_string_n(_error_code, _error_message.data(), _error_message.size());
  std::string _error_output = "OpenSSL error: ";
  _error_output.append(_error_message);
  throw report_exception(_error_output);
}
// LCOV_EXCL_STOP

}  // namespace copper::components
