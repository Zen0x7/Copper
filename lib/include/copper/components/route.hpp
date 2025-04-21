#ifndef COPPER_COMPONENTS_ROUTE_HPP
#define COPPER_COMPONENTS_ROUTE_HPP

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

#include <copper/components/containers.hpp>
#include <copper/components/expression.hpp>
#include <copper/components/method.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {

/**
 * Route result
 */
struct route_result {
  /**
   * Matches
   */
  bool matches_;

  /**
   * Bindings
   */
  containers::unordered_map_of_strings bindings_;
};

/**
 * Route
 */
struct route {
  /**
   * Method
   */
  method method_;

  /**
   * URL
   */
  std::string url_;

  /**
   * Signature
   */
  std::string signature_;

  /**
   * Is expression
   */
  bool is_expression_ = false;

  /**
   * Expression
   */
  shared<expression> expression_;
};

}  // namespace copper::components

#endif
