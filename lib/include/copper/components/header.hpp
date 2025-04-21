#ifndef COPPER_COMPONENTS_HEADER_HPP
#define COPPER_COMPONENTS_HEADER_HPP

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
#include <copper/components/res.hpp>
#include <string>

namespace copper::components {

/**
 * Retrieves headers from request
 *
 * @param req
 * @return string
 */
std::string header_from_request(const req& req);

/**
 * Retrieves headers from response
 *
 * @param res
 * @return string
 */
std::string header_from_response(const res& res);

}  // namespace copper::components

#endif
