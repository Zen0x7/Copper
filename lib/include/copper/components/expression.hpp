#ifndef COPPER_COMPONENTS_EXPRESSION_HPP
#define COPPER_COMPONENTS_EXPRESSION_HPP

#pragma once

#include <boost/smart_ptr.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/shared.hpp>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

namespace copper::components {

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
   * @return
   */
  containers::unordered_map_of_strings get_bindings() const;

  /**
   * Retrieves the value of attribute
   *
   * @param name
   * @return
   */
  std::string get(const std::string &name) const;
};

class expression : public shared_enabled<expression> {
  /**
   * Regex expression
   */
  std::string regex_;

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
   * @return
   */
  containers::vector_of<std::string> get_arguments() const;

  /**
   * Retrieves regex expression
   * @return
   */
  std::string get_regex() const;

  /**
   * Retrieves a expression_result
   *
   * @param input
   * @return
   */
  shared<expression_result> query(const std::string &input) const;
};

/**
 * Factory
 *
 * @param input
 * @return
 */
shared<expression> expression_make(const std::string &input);

}  // namespace copper::components

#endif