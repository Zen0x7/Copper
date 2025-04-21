#ifndef COPPER_COMPONENTS_REQ_HPP
#define COPPER_COMPONENTS_REQ_HPP

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
 * Request
 */
using req = boost::beast::http::request<boost::beast::http::string_body>;

/**
 * Request is illegal
 *
 * @param request
 * @return bool
 */
inline bool request_is_illegal(const req& request) {
  return request.target().empty() || request.target()[0] != '/' ||
         request.target().find("..") != boost::beast::string_view::npos;
}

}  // namespace copper::components

#endif
