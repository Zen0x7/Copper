//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/json/serialize.hpp>
#include <copper/components/header.hpp>

namespace copper::components {

std::string header_from_request(const request& request) {
  json::object _headers;
  for (const auto& _header : request.base()) {
    if (_header.name_string() != "Authorization") {
      _headers[_header.name_string()] = _header.value();
    } else {
      _headers[_header.name_string()] = "***";
    }
  }
  return serialize(_headers);
}

std::string header_from_response(const response& response) {
  json::object _headers;
  for (const auto& _header : response.base()) {
    _headers[_header.name_string()] = _header.value();
  }
  return serialize(_headers);
}
}  // namespace copper::components