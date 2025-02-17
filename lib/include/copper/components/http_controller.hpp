#pragma once

#include <copper/components/http_response.hpp>
#include <copper/components/http_request.hpp>
#include <copper/components/http_status_code.hpp>
#include <copper/components/http_fields.hpp>

#include <copper/components/shared.hpp>

#include <copper/components/containers.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/json.hpp>
#include <copper/components/uuid.hpp>

#include <copper/components/dotenv.hpp>

#include <copper/components/http_controller_config.hpp>

#include <boost/smart_ptr.hpp>
#include <iostream>
#include <unordered_map>
#include <map>

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

      virtual http_response invoke(const http_request & /*request*/) {
        http_response response;
        return response;
      };

      virtual containers::map_of_strings rules() const { return {}; }

      // LCOV_EXCL_STOP

      void set_state(const shared<state> &state);

      void set_config(http_controller_config config);

      void set_start(long at);

      void set_bindings(containers::unordered_map_of_strings &bindings);

      void set_data(const json::value &data);

      void set_user(uuid id);

      http_response response(
        http_request const &request,
        const http_status_code status,
        const std::string &data,
        const char *type = "text/html"
      ) const {
        const auto resolved_at = chronos::now();

        http_response response{};

        response.set(http_fields::content_type, type);
        response.set(http_fields::allow, request.method_string());
        response.set(http_fields::access_control_allow_headers, "Accept,Authorization,Content-Type,X-Requested-With");

        const auto allowed_origins = dotenv::getenv("HTTP_ALLOWED_ORIGINS", "*");

        response.set(http_fields::access_control_allow_origin, allowed_origins);

        response.set("X-Server", "Copper");
        response.set("X-Time", std::to_string(resolved_at - start_));
        response.version(request.version());
        response.keep_alive(request.keep_alive());
        response.result(status);
        response.body() = data;
        response.prepare_payload();
        return response;
      }
    };
}