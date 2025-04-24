// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <boost/json/serialize.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/query.hpp>
#include <istream>
#include <map>
#include <sstream>

namespace copper::components {

std::string query_from_request(const req& request) {
  json::object _object;

  map_of<std::string, vector_of<std::string>> _map;

  const size_t _symbol = request.target().find('?');
  const bool _has_params = _symbol != std::string::npos;
  const std::string _query{_has_params ? request.target().substr(_symbol + 1)
                                       : ""};

  std::istringstream _ss(_query);
  std::string _pair;

  while (std::getline(_ss, _pair, '&')) {
    const size_t _pos = _pair.find('=');
    if (_pos == std::string::npos)
      continue;

    std::string _key = _pair.substr(0, _pos);
    std::string _value = _pair.substr(_pos + 1);

    if (_key.size() > 2 && _key.ends_with("[]")) {
      _key = _key.substr(0, _key.size() - 2);
      _map[_key].push_back(_value);
    } else {
      _object[_key] = _value;
    }
  }

  for (const auto& [key, values] : _map) {
    json::array array;
    for (const auto& value : values) {
      array.push_back(json::string(value));
    }
    _object[key] = array;
  }

  return std::string(serialize(_object));
}
}  // namespace copper::components
