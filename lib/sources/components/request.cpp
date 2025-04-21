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

#include <copper/components/request.hpp>
#include <copper/components/url.hpp>

namespace copper::components {

shared<request> request_from_req(const shared<connection>& connection,
                                 long now,
                                 const req& req) {
  return boost::make_shared<request>(
      connection, std::to_string(req.version()),
      std::string(req.method_string()), url_from_request(req),
      query_from_request(req), header_from_request(req),
      std::string(req.body()), now, 0, 0);
}
}  // namespace copper::components
