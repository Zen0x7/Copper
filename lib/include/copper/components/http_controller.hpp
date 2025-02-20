#ifndef COPPER_COMPONENTS_HTTP_CONTROLLER_HPP
#define COPPER_COMPONENTS_HTTP_CONTROLLER_HPP

#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/json/serialize.hpp>
#include <boost/smart_ptr.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/dotenv.hpp>
#include <copper/components/http_controller_config.hpp>
#include <copper/components/http_fields.hpp>
#include <copper/components/http_request.hpp>
#include <copper/components/http_response.hpp>
#include <copper/components/http_status_code.hpp>
#include <copper/components/json.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>

namespace copper::components {

class state;

class http_controller : public shared_enabled<http_controller> {
 public:
  containers::unordered_map_of_strings bindings_;
  json::value data_;
  shared<state> state_;
  uuid auth_id_;
  long start_ = 0;
  http_controller_config config_;

  // LCOV_EXCL_START
  virtual ~http_controller() = default;

  virtual containers::async_of<http_response> invoke(
      const http_request & /*request*/) {
    http_response response;
    co_return response;
  };

  virtual containers::map_of_strings rules() const { return {}; }

  // LCOV_EXCL_STOP

  void set_state(const shared<state> &state);

  void set_config(http_controller_config config);

  void set_start(long at);

  void set_bindings(containers::unordered_map_of_strings &bindings);

  void set_data(const json::value &data);

  void set_user(uuid id);

  http_response response(http_request const &request, http_status_code status,
                         const std::string &data,
                         const char *type = "text/html") const;
};

}  // namespace copper::components

#endif