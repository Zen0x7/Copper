#include <copper/components/url.hpp>

namespace copper::components {

std::string url_from_request(const request &request) {
  const size_t _symbol = request.target().find('?');
  const bool _has_params = _symbol != std::string::npos;
  return std::string{_has_params ? request.target().substr(0, _symbol)
                                 : request.target()};
}
}  // namespace copper::components