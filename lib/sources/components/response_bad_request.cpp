//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/response_bad_request.hpp>
#include <copper/components/response_shared_handler.hpp>
#include <copper/components/status_code.hpp>

namespace copper::components {

response response_bad_request(const request &request, const long start_at) {
  response _response{status_code::bad_request, request.version()};

  return response_shared_handler(request, _response, start_at);
}
}  // namespace copper::components