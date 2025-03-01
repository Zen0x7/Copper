//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/url.hpp>
#include <copper/models/request.hpp>

namespace copper::models {

components::shared<request> request_from_request(
    const components::uuid session_id, const components::uuid request_id,
    long now, const components::request &request) {
  return boost::make_shared<models::request>(
      to_string(request_id), to_string(session_id),
      std::to_string(request.version()), std::string(request.method_string()),
      components::url_from_request(request),
      components::query_from_request(request),
      components::header_from_request(request), std::string(request.body()),
      now, 0, 0);
}
}  // namespace copper::models