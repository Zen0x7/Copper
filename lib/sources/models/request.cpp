#include <copper/models/request.hpp>

namespace copper::models {

components::shared<request> request_from_http_request(
    components::uuid session_id, components::uuid request_id, long now,
    const components::http_request &request) {
  return boost::make_shared<copper::models::request>(
      to_string(request_id), to_string(session_id),
      std::to_string(request.version()), std::string(request.method_string()),
      components::http_path_from_request(request),
      components::http_query_from_request(request),
      components::http_header_from_request(request),
      std::string(request.body()), now, 0, 0);
}
}  // namespace copper::models