#ifndef COPPER_COMPONENTS_ROUTE_HPP
#define COPPER_COMPONENTS_ROUTE_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

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
