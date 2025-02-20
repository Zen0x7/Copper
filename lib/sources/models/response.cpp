#include <app/models/response.hpp>

namespace app::models {

    shared<response> response_from_http_response(const shared<session> &session, const shared<request> &request,
                                                 const http_response &http_response) {
      return boost::make_shared<response>(
        to_string(boost::uuids::random_generator()()),
        session->id_,
        request->id_,
        http_response.result_int(),
        http_header_from_response(http_response),
        http_response.body()
      );
    }
}