//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <openssl/err.h>

#include <boost/stacktrace.hpp>
#include <copper/components/report.hpp>
#include <iostream>

namespace copper::components {

void report(const boost::beast::error_code& ec, const char* what) {
  std::cerr << boost::stacktrace::stacktrace() << '\n';
  std::cerr << what << ": " << ec.message() << "\n";
}

// LCOV_EXCL_START
void report_for_openssl() {
  const unsigned long _error_code = ERR_get_error();
  std::string _error_message(256, '\0');
  ERR_error_string_n(_error_code, _error_message.data(), _error_message.size());
  std::string _error_output = "OpenSSL error: ";
  _error_output.append(_error_message);
  throw report_exception(_error_output.data());
}

// LCOV_EXCL_STOP

}  // namespace copper::components
