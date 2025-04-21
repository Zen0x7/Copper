#ifndef COPPER_COMPONENTS_MIME_TYPE_HPP
#define COPPER_COMPONENTS_MIME_TYPE_HPP

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

#include <boost/beast/core/string.hpp>

namespace copper::components {

/**
 * Get MIME type
 *
 * @param path
 * @return string Output
 */
boost::beast::string_view mime_type(boost::beast::string_view path);

}  // namespace copper::components

#endif
