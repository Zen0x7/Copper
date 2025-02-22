#ifndef COPPER_COMPONENTS_HTTP_ROUTE_HPP
#define COPPER_COMPONENTS_HTTP_ROUTE_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/expression.hpp>
#include <copper/components/http_method.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {

/**
 * HTTP route result
 */
struct http_route_result {
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
 * HTTP route
 */
struct http_route {
  /**
   * Method
   */
  http_method method_;

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
