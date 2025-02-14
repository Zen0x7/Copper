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

#include <boost/smart_ptr.hpp>
#include <boost/json.hpp>
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

        virtual ~http_controller() = default;

        virtual http_response invoke(const http_request &request);

        virtual void prepare(
                http_request &request,
                json::object &errors
        ) const {
            std::cout << request.body() << std::endl;
            std::cout << serialize(errors) << std::endl;
        }

        virtual containers::map_of_strings validate_data() const { return {}; }

        virtual bool requires_data() const { return false; }

        virtual bool requires_authentication() const { return false; }

        virtual bool requires_limitation() const { return false; }

        virtual int requests_per_minute() const { return 0; }


        void set_state(const shared<state> &state);

        void set_bindings(containers::unordered_map_of_strings &bindings);

        void set_data(const json::value &data);

        void set_user(uuid id);

        http_response static response(
                http_request const &request,
                const http_status_code status,
                const std::string &data,
                const char *type = "text/html",
                const long started_at = 0
        ) {
            const auto resolved_at = chronos::now();

            http_response response{};

            response.set(http_fields::content_type, type);
            response.set(http_fields::allow, request.method_string());
            response.set(http_fields::access_control_allow_headers, "Accept,Authorization,Content-Type,X-Requested-With");

            const auto allowed_origins = dotenv::getenv("HTTP_ALLOWED_ORIGINS", "*");

            response.set(http_fields::access_control_allow_origin, allowed_origins);

            response.set("X-Server", "Copper");
            response.set("X-Time", std::to_string(resolved_at - started_at));
            response.version(request.version());
            response.keep_alive(request.keep_alive());
            response.result(status);
            response.body() = data;
            response.prepare_payload();
            return response;
        }
    };
}