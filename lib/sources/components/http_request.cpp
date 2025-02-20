#include <copper/components/http_request.hpp>

namespace copper::components {

bool http_request_is_illegal(const http_request &request) {
  return request.target().empty() || request.target()[0] != '/' ||
         request.target().find("..") != boost::beast::string_view::npos;
}
}  // namespace copper::components