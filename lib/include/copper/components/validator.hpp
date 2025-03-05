#ifndef COPPER_COMPONENTS_VALIDATOR_HPP
#define COPPER_COMPONENTS_VALIDATOR_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/containers.hpp>
#include <copper/components/json.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {

/**
 * Validator
 */
class validator : public shared_enabled<validator> {
 public:
  /**
   * Errors
   */
  json::object errors_;

  /**
   * Status
   */
  bool success_ = false;

  /**
   * Insert or push message to errors
   *
   * @param key
   * @param message
   */
  void insert_or_push(const std::string &key, const std::string &message);

  /**
   * Constructor
   */
  validator() = default;

  /**
   * Per rule
   *
   * @param value
   * @param attribute
   * @param rule
   */
  bool per_rule(const json::value &value, const std::string &attribute,
                const std::string &rule);

  /**
   * Per scope rule
   *
   * @param value
   * @param attribute
   * @param rule
   * @return
   */
  bool per_scope_rule(const json::value &value, const std::string &attribute,
                      const std::string &rule);

  /**
   * On confirmation rule
   *
   * @param value
   * @param attribute
   */
  void on_confirmation_rule(const json::value &value,
                            const std::string &attribute);

  /**
   * On array of strings rule
   * @param value
   * @param attribute
   */
  void on_array_of_strings_rule(const json::value &value,
                                const std::string &attribute);

  /**
   * On array of strings per element rule
   * @param value
   * @param attribute
   */
  void on_array_of_strings_per_element_rule(const json::value &value,
                                            const std::string &attribute);

  /**
   * On number rule
   *
   * @param value
   * @param attribute
   */
  void on_number_rule(const json::value &value, const std::string &attribute);

  /**
   * On object rule
   *
   * @param value
   * @param attribute
   */
  void on_object_rule(const json::value &value, const std::string &attribute);

  /**
   * On uuid rule
   *
   * @param value
   * @param attribute
   */
  void on_uuid_rule(const json::value &value, const std::string &attribute);

  /**
   * On string rule
   *
   * @param value
   * @param attribute
   */
  void on_string_rule(const json::value &value, const std::string &attribute);
};

/**
 * Validator factory
 *
 * @param rules
 * @param value
 * @return boost::shared_ptr<validator> validator
 */
shared<validator> validator_make(const containers::map_of_strings &rules,
                                 const json::value &value);

}  // namespace copper::components

#endif
