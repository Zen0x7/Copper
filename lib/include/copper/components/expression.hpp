#ifndef COPPER_COMPONENTS_EXPRESSION_HPP
#define COPPER_COMPONENTS_EXPRESSION_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/containers.hpp>
#include <copper/components/shared.hpp>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

namespace copper::components {

// LCOV_EXCL_START
class expression_exception final : public std::runtime_error {
 public:
  explicit expression_exception(const std::string &what = "")
      : std::runtime_error(what) {}
};
// LCOV_EXCL_STOP

/**
 * Expression result
 */
class expression_result : public shared_enabled<expression_result> {
  /**
   * Matches flag
   */
  bool matches_;

  /**
   * Bindings
   */
  containers::unordered_map_of_strings bindings_;

 public:
  /**
   * Constructor
   *
   * @param matches
   * @param bindings
   */
  expression_result(bool matches,
                    const containers::unordered_map_of_strings &bindings);

  /**
   * Retrieves if matches
   *
   * @return
   */
  bool matches() const;

  /**
   * Retrieves the bindings
   *
   * @return unordered_map_of_strings
   */
  containers::unordered_map_of_strings get_bindings() const;

  /**
   * Retrieves the value of attribute
   *
   * @param name
   * @return string
   */
  std::string get(const std::string &name) const;
};

/**
 * Expression
 */
class expression : public shared_enabled<expression> {
  /**
   * Regex
   */
  std::string regex_;

  /**
   * Pattern
   */
  std::regex pattern_;

  /**
   * Arguments
   */
  containers::vector_of<std::string> arguments_;

 public:
  /**
   * Constructor
   *
   * @param regex
   * @param arguments
   */
  expression(std::string regex,
             const containers::vector_of<std::string> &arguments);

  /**
   * Retrieves arguments
   *
   * @return vector<string>
   */
  containers::vector_of<std::string> get_arguments() const;

  /**
   * Retrieves regex expression
   * @return string
   */
  std::string get_regex() const;

  /**
   * Retrieves a expression_result
   *
   * @param input
   * @return shared<expression_result>
   */
  shared<expression_result> query(const std::string &input) const;
};

/**
 * Expression factory
 *
 * @param input
 * @return shared<expression>
 */
shared<expression> expression_make(const std::string &input);

}  // namespace copper::components

#endif