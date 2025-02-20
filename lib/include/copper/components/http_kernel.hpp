#ifndef COPPER_COMPONENTS_HTTP_KERNEL_HPP
#define COPPER_COMPONENTS_HTTP_KERNEL_HPP

#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/strand.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/http_request.hpp>
#include <copper/components/http_response.hpp>
#include <copper/components/http_response_generic.hpp>
#include <copper/components/http_route.hpp>
#include <copper/components/normalized_path.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>
#include <copper/models/request.hpp>
#include <copper/models/response.hpp>
#include <copper/models/session.hpp>

#ifndef HTTP_SERVER_HEADER_CONTENT
#define HTTP_SERVER_HEADER_CONTENT "Copper"
#endif

namespace copper::components {
class state;

class http_controller;

struct http_kernel_result {
  http_route route_;
  shared<http_controller> controller_;
  containers::unordered_map_of_strings bindings_;
};

class http_kernel : public shared_enabled<http_kernel> {
  shared<state> state_;

 public:
  explicit http_kernel(const shared<state> &state) : state_(state) {}

  containers::optional_of<http_kernel_result> find_on_routes(
      const http_request &request) const;

  containers::vector_of<http_method> get_available_methods(
      const http_request &request) const;

  containers::async_of<
      std::tuple<shared<copper::models::request>,
                 shared<copper::models::response>, http_response_generic>>
  invoke(uuid session_id, boost::beast::string_view /* root */,
         const http_request &request, const std::string &ip,
         const uuid &request_id, long now) const;
};

}  // namespace copper::components

#endif