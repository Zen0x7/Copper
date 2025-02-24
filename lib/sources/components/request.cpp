#include <copper/components/request.hpp>

namespace copper::components {

bool request_is_illegal(const request &request) {
  return request.target().empty() || request.target()[0] != '/' ||
         request.target().find("..") != boost::beast::string_view::npos;
}
}  // namespace copper::components