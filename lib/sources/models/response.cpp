#include <copper/components/uuid.hpp>
#include <copper/models/response.hpp>

namespace copper::models {

components::shared<response> response_from_response(
    components::uuid session_id, const components::shared<request> &request,
    const components::response &http_response) {
  return boost::make_shared<response>(
      to_string(boost::uuids::random_generator()()), to_string(session_id),
      request->id_, http_response.result_int(),
      components::header_from_response(http_response), http_response.body());
}
}  // namespace copper::models