#pragma once

#include <string>

#include <copper/components/http_request.hpp>
#include <copper/components/json.hpp>

namespace copper::components {
    std::string http_header_from_request(const http_request & request) {
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
}