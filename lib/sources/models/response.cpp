//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/uuid/random_generator.hpp>
#include <copper/components/uuid.hpp>
#include <copper/models/response.hpp>

namespace copper::models {

components::shared<response> response_from_response(
    const components::uuid session_id,
    const components::shared<request> &request,
    const components::response &service_response) {
  return boost::make_shared<response>(
      to_string(boost::uuids::random_generator()()), to_string(session_id),
      request->id_, service_response.result_int(),
      components::header_from_response(service_response),
      service_response.body());
}
}  // namespace copper::models