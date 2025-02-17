#pragma once

#include <copper/components/http_controller.hpp>
#include <copper/components/json.hpp>
#include <copper/components/cipher.hpp>

namespace app::controllers {
    class user_controller final : public copper::components::http_controller {
    public:
      copper::components::http_response invoke(const copper::components::http_request &request) override {
        const copper::components::json::object data = {{"data",to_string(this->auth_id_)}};
        return response(request, copper::components::http_status_code::ok, serialize(data), "application/json");
      }
    };

} // namespace app::controller