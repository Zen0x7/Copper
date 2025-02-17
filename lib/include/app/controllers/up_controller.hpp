#pragma once

#include <copper/components/http_controller.hpp>
#include <copper/components/json.hpp>
#include <copper/components/cipher.hpp>

namespace app::controllers {
    class up_controller final : public copper::components::http_controller {
    public:
      copper::components::http_response invoke(const copper::components::http_request &request) override {
        auto now = copper::components::chronos::now();
        const copper::components::json::object data = {{"timestamp",now}};
        return response(request, copper::components::http_status_code::ok, serialize(data), "application/json");
      }
    };

} // namespace app::controller