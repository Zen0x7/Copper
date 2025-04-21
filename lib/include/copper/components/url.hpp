#ifndef COPPER_COMPONENTS_URL_HPP
#define COPPER_COMPONENTS_URL_HPP

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

#include <copper/components/req.hpp>

namespace copper::components {
/**
 * URL from request
 *
 * @param request
 * @return string
 */
inline std::string url_from_request(const req& request) {
  const size_t _symbol = request.target().find('?');
  const bool _has_params = _symbol != std::string::npos;
  return std::string{_has_params ? request.target().substr(0, _symbol)
                                 : request.target()};
}
}  // namespace copper::components

#endif
