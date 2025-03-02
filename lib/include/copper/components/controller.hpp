#ifndef COPPER_COMPONENTS_CONTROLLER_HPP
#define COPPER_COMPONENTS_CONTROLLER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/atomic.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/controller_configuration.hpp>
#include <copper/components/controller_parameters.hpp>
#include <copper/components/json.hpp>
#include <copper/components/response.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/status_code.hpp>
#include <copper/components/uuid.hpp>

namespace copper::components {
/**
 * Controller
 */
class controller : public shared_enabled<controller> {
 public:
  /**
   * Destructor
   */
  virtual ~controller() = default;

  /**
   * Auth ID
   */
  uuid auth_id_;

  /**
   * Start at
   */
  long start_at_ = 0;

  /**
   * Count
   */
  boost::atomic<unsigned long long> count_;

  /**
   * Configuration
   */
  controller_configuration configuration_;

  // LCOV_EXCL_START

  /**
   * Invoke
   *
   * @return async_of<response>
   */
  virtual containers::async_of<response> invoke(
      const shared<controller_parameters> /*parameters*/) {
    response response;
    co_return response;
  };

  /**
   * Rules
   *
   * @return map_of_strings
   */
  virtual containers::map_of_strings rules() const { return {}; }

  // LCOV_EXCL_STOP

  /**
   * Set configuration
   *
   * @param configuration
   */
  void set_configuration(controller_configuration configuration);

  /**
   * Make response
   *
   * @param parameters
   * @param status
   * @param data
   * @param type
   * @return response
   */
  response make_response(const shared<controller_parameters> &parameters,
                         status_code status, const std::string &data,
                         const char *type = "text/html") const;

  /**
   * Make view
   *
   * @param parameters
   * @param status
   * @param view
   * @param data
   * @param type
   * @return response
   */
  response make_view(

      const shared<controller_parameters> &parameters, status_code status,
      const std::string &view, const json::json &data,
      const char *type = "text/html") const;
};

}  // namespace copper::components

#endif