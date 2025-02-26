#include <copper/components/url.hpp>

namespace copper::components {

/**
 * URL from request
 *
 * @param request
 * @return string
 */
std::string url_from_request(const request &request) {
  const size_t symbol = request.target().find('?');
  const bool has_params = symbol != std::string::npos;
  return std::string{has_params ? request.target().substr(0, symbol)
                                : request.target()};
}
}  // namespace copper::components