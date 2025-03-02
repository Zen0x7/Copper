//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/response_shared_handler.hpp>
#include <copper/components/response_too_many_requests.hpp>
#include <copper/components/status_code.hpp>

namespace copper::components {

response response_too_many_requests(const request &request, const long start_at,
                                    const int ttl) {
  response _response{status_code::too_many_requests, request.version()};
  return response_shared_handler(request, _response, start_at, ttl);
}
}  // namespace copper::components