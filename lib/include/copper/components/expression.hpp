#ifndef COPPER_COMPONENTS_EXPRESSION_HPP
#define COPPER_COMPONENTS_EXPRESSION_HPP

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
#include <copper/components/shared.hpp>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

namespace copper::components {

/**
 * Expression exception
 */
class expression_exception final : public std::runtime_error {
 public:
  /**
   * Constructor
   * @param what
   */
  explicit expression_exception(const std::string& what = "")
      : std::runtime_error(what) {}
};

/**
 * Expression result
 */
class expression_result : public shared_enabled<expression_result> {
  /**
   * Matches flag
   */
  bool matches_;

 public:
  /**
   * Bindings
   */
  unordered_map_of_strings bindings_;

  /**
   * Constructor
   *
   * @param matches
   * @param bindings
   */
  expression_result(const bool matches,
                    const unordered_map_of_strings& bindings)
      : matches_(matches), bindings_(bindings){};

  /**
   * Retrieves if matches
   *
   * @return
   */
  bool matches() const { return matches_; };

  /**
   * Retrieves the bindings
   *
   * @return unordered_map_of_strings
   */
  unordered_map_of_strings get_bindings() const { return bindings_; }

  /**
   * Retrieves the value of attribute
   *
   * @param name
   * @return string
   */
  std::string get(const std::string& name) const;
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
  vector_of<std::string> arguments_;

 public:
  /**
   * Constructor
   *
   * @param regex
   * @param arguments
   */
  expression(std::string regex, const vector_of<std::string>& arguments)
      : regex_(std::move(regex)), pattern_(regex_), arguments_(arguments){};

  /**
   * Retrieves arguments
   *
   * @return vector<string>
   */
  vector_of<std::string> get_arguments() const { return arguments_; };

  /**
   * Retrieves regex expression
   * @return string
   */
  std::string get_regex() const { return regex_; };

  /**
   * Retrieves a expression_result
   *
   * @param input
   * @return shared<expression_result>
   */
  shared<expression_result> query(const std::string& input) const;
};

/**
 * Expression factory
 *
 * @param input
 * @return shared<expression>
 */
shared<expression> expression_make(const std::string& input);

}  // namespace copper::components

#endif
