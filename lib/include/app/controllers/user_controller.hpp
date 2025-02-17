#pragma once

#include <copper/components/http_controller.hpp>
#include <copper/components/json.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/chronos.hpp>

namespace app::controllers {
    class user_controller final : public copper::components::http_controller {
    public:
      copper::components::http_response invoke(const copper::components::http_request &request) override {
        auto _user = state_->get_database()->get_user_by_id(auth_id_);

        const copper::components::json::value data = boost::json::value_from(_user);

        return response(request, copper::components::http_status_code::ok, serialize(data), "application/json");
      }
    };

} // namespace app::controller