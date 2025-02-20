#include <copper/models/response.hpp>

namespace copper::models {

    components::shared<response> response_from_http_response(const components::shared<session> &session, const components::shared<request> &request,
                                                 const components::http_response &http_response) {
      return boost::make_shared<response>(
        to_string(boost::uuids::random_generator()()),
        session->id_,
        request->id_,
        http_response.result_int(),
        components::http_header_from_response(http_response),
        http_response.body()
      );
    }
}