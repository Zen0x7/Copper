#pragma once

#include <copper/components/json.hpp>
#include <string>
#include <map>
#include <vector>
#include <istream>
#include <sstream>

namespace copper::components {

    json::object http_query_to_json(const std::string & query) {
        json::object obj;
        std::map<std::string, std::vector<std::string>> arrayMap;

        std::istringstream ss(query.data());
        std::string pair;

        while (std::getline(ss, pair, '&')) {
          size_t pos = pair.find('=');
          if (pos == std::string::npos) continue;

          std::string key = pair.substr(0, pos);
          std::string value = pair.substr(pos + 1);

          if (key.size() > 2 && key.substr(key.size() - 2) == "[]") {
            key = key.substr(0, key.size() - 2);
            arrayMap[key].push_back(value);
          } else {
            obj[key] = value;
          }
        }

        for (const auto& [key, values] : arrayMap) {
          json::array jsonArray;
          for (const auto& val : values) {
            jsonArray.push_back(json::string(val));
          }
          obj[key] = jsonArray;
        }

        return obj;
    }
}