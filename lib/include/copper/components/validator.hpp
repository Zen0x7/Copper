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
