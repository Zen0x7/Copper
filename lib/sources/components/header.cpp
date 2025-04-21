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
#include <copper/components/header.hpp>

namespace copper::components {

std::string header_from_request(const req& req) {
  json::object _headers;
  for (const auto& _header : req.base()) {
    if (_header.name_string() != "Authorization") {
      _headers[_header.name_string()] = _header.value();
    } else {
      _headers[_header.name_string()] = "***";
    }
  }
  return serialize(_headers);
}

std::string header_from_response(const res& res) {
  json::object _headers;
  for (const auto& _header : res.base()) {
    _headers[_header.name_string()] = _header.value();
  }
  return serialize(_headers);
}
}  // namespace copper::components
