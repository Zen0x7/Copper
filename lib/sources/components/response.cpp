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

#include <boost/uuid/random_generator.hpp>
#include <copper/components/connection.hpp>
#include <copper/components/response.hpp>
#include <copper/components/uuid.hpp>
#include <iostream>

namespace copper::components {
shared<response> response_factory(const shared<connection>& connection,
                                  const shared<request>& request,
                                  const res& res) {
  return boost::make_shared<response>(
      connection->id_, request->id_, res.result_int(),
      header_from_response(res), res.body(), !res["Content-Encoding"].empty());
}
}  // namespace copper::components
