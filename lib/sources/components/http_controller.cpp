#include <copper/components/http_controller.hpp>

namespace copper::components {

    void http_controller::set_state(const shared<state> &state) { state_ = state; }

    void http_controller::set_bindings(containers::unordered_map_of_strings &bindings) {
        bindings_ = std::move(bindings);
    }

    // LCOV_EXCL_START
    void http_controller::set_data(const json::value &data) { data_ = data; }

    void http_controller::set_user(const uuid id) { auth_id_ = id; }
    // LCOV_EXCL_STOP
}