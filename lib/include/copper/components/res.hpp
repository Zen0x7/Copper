#ifndef COPPER_COMPONENTS_RES_HPP
#define COPPER_COMPONENTS_RES_HPP

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

#include <boost/beast/http/string_body.hpp>

namespace copper::components {

/**
 * Response
 */
using res = boost::beast::http::response<boost::beast::http::string_body>;

}  // namespace copper::components

#endif
