#ifndef COPPER_COMPONENTS_ROUTE_FIND_HPP
#define COPPER_COMPONENTS_ROUTE_FIND_HPP

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

#include <copper/components/router.hpp>

namespace copper::components {

/**
 * Route find
 *
 * @param url
 * @param route
 * @return route_result
 */
inline route_result route_find(const std::string& url, const route& route) {
  if (route.is_expression_) {
    const auto _result = route.expression_->query(url);
    return {
        .matches_ = _result->matches(),
        .bindings_ = _result->get_bindings(),
    };
  }

  if (route.signature_ == url)
    return {.matches_ = true, .bindings_ = {}};

  return {
      .matches_ = false,
      .bindings_ = {},
  };
}

}  // namespace copper::components

#endif
