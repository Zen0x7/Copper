#ifndef COPPER_COMPONENTS_HTTP_QUERY_HPP
#define COPPER_COMPONENTS_HTTP_QUERY_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/http_request.hpp>
#include <copper/components/json.hpp>
#include <istream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace copper::components {

std::string http_query_from_request(const http_request &request) {
  json::object object;

  containers::map_of<std::string, containers::vector_of<std::string>> map;

  const size_t query_ask_symbol_position = request.target().find('?');
  const bool path_has_parameters =
      query_ask_symbol_position != std::string::npos;
  const std::string query{
      path_has_parameters
          ? request.target().substr(query_ask_symbol_position + 1)
          : ""};

  std::istringstream ss(query);
  std::string pair;

  while (std::getline(ss, pair, '&')) {
    size_t pos = pair.find('=');
    if (pos == std::string::npos) continue;

    std::string key = pair.substr(0, pos);
    std::string value = pair.substr(pos + 1);

    if (key.size() > 2 && key.substr(key.size() - 2) == "[]") {
      key = key.substr(0, key.size() - 2);
      map[key].push_back(value);
    } else {
      object[key] = value;
    }
  }

  for (const auto &[key, values] : map) {
    json::array array;
    for (const auto &value : values) {
      array.push_back(json::string(value));
    }
    object[key] = array;
  }

  return std::string(serialize(object));
}
}  // namespace copper::components

#endif