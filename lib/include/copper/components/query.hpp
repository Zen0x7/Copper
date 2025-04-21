#ifndef COPPER_COMPONENTS_QUERY_HPP
#define COPPER_COMPONENTS_QUERY_HPP

#pragma once

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

#include <copper/components/json.hpp>
#include <copper/components/req.hpp>
#include <string>

namespace copper::components {

/**
 * Query from request
 *
 * @param request
 * @return string
 */
std::string query_from_request(const req& request);

}  // namespace copper::components

#endif
