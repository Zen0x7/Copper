#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <copper/components/report.hpp>

namespace copper::components {
void report(boost::beast::error_code ec, const char *what) {
  std::cerr << boost::stacktrace::stacktrace() << '\n';
  std::cerr << what << ": " << ec.message() << "\n";
}

// LCOV_EXCL_START
void report_for_openssl() {
  unsigned long error_code = ERR_get_error();
  char error_message[256];
  ERR_error_string_n(error_code, error_message, sizeof(error_message));
  std::string error_output = "OpenSSL error: ";
  error_output.append(error_message);
  throw std::runtime_error(error_output.c_str());
}
// LCOV_EXCL_STOP

}  // namespace copper::components
