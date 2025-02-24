#include <copper/components/header.hpp>

namespace copper::components {

std::string header_from_request(const request& request) {
  json::object headers;
  for (const auto& header : request.base()) {
    if (header.name_string() != "Authorization") {
      headers[header.name_string()] = header.value();
    } else {
      headers[header.name_string()] = "***";
    }
  }
  return serialize(headers);
}

std::string header_from_response(const response& response) {
  json::object headers;
  for (const auto& header : response.base()) {
    headers[header.name_string()] = header.value();
  }
  return serialize(headers);
}
}  // namespace copper::components