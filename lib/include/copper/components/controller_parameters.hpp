#ifndef COPPER_COMPONENTS_CONTROLLER_PARAMETERS_HPP
#define COPPER_COMPONENTS_CONTROLLER_PARAMETERS_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/authentication.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/json.hpp>
#include <copper/components/request.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {
class controller_parameters : public shared_enabled<controller_parameters> {
  /**
   * Request
   */
  request request_;

  /**
   * Body
   */
  json::value body_;

  /**
   * Auth
   */
  containers::optional_of<authentication_result> auth_;

  /**
   * Bindings
   */
  containers::unordered_map_of_strings bindings_;

  /**
   * Start at
   */
  long start_at_;

 public:
  /**
   * Constructor
   */
  controller_parameters(
      const request& request, const json::value& body,
      const containers::optional_of<authentication_result>& auth,
      const containers::unordered_map_of_strings& bindings,
      const long& start_at);

  /**
   * Get request
   *
   * @return request
   */
  const request& get_request() const;

  /**
   * Get body
   *
   * @return json::value
   */
  const json::value& get_body() const;

  /**
   * Get auth
   *
   * @return optional_of<authentication_result>
   */
  const containers::optional_of<authentication_result>& get_auth() const;

  /**
   * Get bindings
   *
   * @return unordered_map_of_strings
   */
  const containers::unordered_map_of_strings& get_bindings() const;

  /**
   * Get start at
   *
   * @return long
   */
  const long& get_start_at() const;
};

}  // namespace copper::components

#endif
