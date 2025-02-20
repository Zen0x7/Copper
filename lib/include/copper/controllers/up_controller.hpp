#pragma once

#include <copper/components/cipher.hpp>
#include <copper/components/http_controller.hpp>
#include <copper/components/json.hpp>

namespace copper::controllers {
class up_controller final : public copper::components::http_controller {
 public:
  boost::asio::awaitable<
      copper::components::http_response,
      boost::asio::strand<boost::asio::io_context::executor_type>>
  invoke(const copper::components::http_request &request) override {
    auto now = copper::components::chronos::now();
    const copper::components::json::object data = {{"timestamp", now}};
    co_return response(request, copper::components::http_status_code::ok,
                       serialize(data), "application/json");
  }
};

}  // namespace copper::controllers