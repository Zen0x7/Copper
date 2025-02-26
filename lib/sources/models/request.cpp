#include <copper/models/request.hpp>

namespace copper::models {

components::shared<request> request_from_request(
    components::uuid session_id, components::uuid request_id, long now,
    const components::request &request) {
  return boost::make_shared<models::request>(
      to_string(request_id), to_string(session_id),
      std::to_string(request.version()), std::string(request.method_string()),
      components::path_from_request(request),
      components::query_from_request(request),
      components::header_from_request(request), std::string(request.body()),
      now, 0, 0);
}
}  // namespace copper::models