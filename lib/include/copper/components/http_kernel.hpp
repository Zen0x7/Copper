#pragma once

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast

#include <boost/asio/awaitable.hpp>
#include <boost/asio/strand.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/http_request.hpp>
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

  boost::asio::awaitable<
      std::tuple<shared<copper::models::request>,
                 shared<copper::models::response>, http_response_generic>,
      boost::asio::strand<boost::asio::io_context::executor_type> >
  invoke(const shared<copper::models::session> &session,
         boost::beast::string_view /* root */, const http_request &request,
         const std::string &ip, const uuid &request_id, long now) const;
};

}  // namespace copper::components