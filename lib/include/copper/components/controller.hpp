#ifndef COPPER_COMPONENTS_CONTROLLER_HPP
#define COPPER_COMPONENTS_CONTROLLER_HPP

#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/json/serialize.hpp>
#include <boost/smart_ptr.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/controller_configuration.hpp>
#include <copper/components/dotenv.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/request.hpp>
#include <copper/components/response.hpp>
#include <copper/components/json.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/status_code.hpp>
#include <copper/components/uuid.hpp>

namespace copper::components {

class state;

class controller : public shared_enabled<controller> {
 public:
  /**
   * Bindings
   */
  containers::unordered_map_of_strings bindings_;

  /**
   * Body
   */
  json::value body_;

  /**
   * State
   */
  shared<state> state_;

  /**
   * Auth ID
   */
  uuid auth_id_;

  /**
   * Start at
   */
  long start_at_ = 0;

  /**
   * Configuration
   */
  controller_configuration configuration_;

  // LCOV_EXCL_START
  /**
   * Constructor
   */
  virtual ~controller() = default;

  /**
   * Invoke
   *
   * @return async_of<response>
   */
  virtual containers::async_of<response> invoke(
      const request & /*request*/) {
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
   * Set state
   *
   * @param state
   */
  void set_state(const shared<state> &state);

  /**
   * Set configuration
   *
   * @param configuration
   */
  void set_configuration(controller_configuration configuration);

  /**
   * Set start at
   *
   * @param start_at
   */
  void set_start_at(long start_at);

  /**
   * Set bindings
   *
   * @param bindings
   */
  void set_bindings(containers::unordered_map_of_strings &bindings);

  /**
   * Set body
   *
   * @param body
   */
  void set_body(const json::value &body);

  /**
   * Set user
   *
   * @param id
   */
  void set_user(uuid id);

  /**
   * Make response
   *
   * @param request
   * @param status
   * @param data
   * @param type
   * @return response
   */
  response make_response(const request &request, status_code status,
                         const std::string &data,
                         const char *type = "text/html") const;

  /**
   * Make view
   *
   * @param request
   * @param status
   * @param data
   * @param type
   * @return response
   */
  response make_view(const request &request, status_code status,
                     const std::string view, const json::json &data,
                     const char *type = "text/html") const;
};

}  // namespace copper::components

#endif