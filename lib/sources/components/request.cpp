//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/request.hpp>

namespace copper::components {

bool request_is_illegal(const request &request) {
  return request.target().empty() || request.target()[0] != '/' ||
         request.target().find("..") != boost::beast::string_view::npos;
}
}  // namespace copper::components