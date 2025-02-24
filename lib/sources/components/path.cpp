#include <copper/components/path.hpp>

namespace copper::components {

std::string path_from_request(const request &request) {
  const size_t query_ask_symbol_position = request.target().find('?');
  const bool path_has_parameters =
      query_ask_symbol_position != std::string::npos;
  const std::string path{path_has_parameters ? request.target().substr(
                                                   0, query_ask_symbol_position)
                                             : request.target()};

  return path;
}
}  // namespace copper::components