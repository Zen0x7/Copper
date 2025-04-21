#ifndef COPPER_COMPONENTS_KERNEL_HPP
#define COPPER_COMPONENTS_KERNEL_HPP

#pragma once

// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <copper/components/containers.hpp>
#include <copper/components/core.hpp>
#include <copper/components/req.hpp>
#include <copper/components/request.hpp>
#include <copper/components/response.hpp>
#include <copper/components/response_generic.hpp>
#include <copper/components/route.hpp>
#include <copper/components/route_find.hpp>
#include <copper/components/route_match.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>

namespace copper::components {
using namespace containers;

/**
 * Forward controller
 */
class controller;

/**
 * Kernel call result
 */
using kernel_call_result =
    std::tuple<shared<request>, shared<response>, response_generic>;

/**
 * Kernel result
 */
struct kernel_result {
  route route_;
  shared<controller> controller_;
  unordered_map_of_strings bindings_;
};

/**
 * Kernel
 */
class kernel : public shared_enabled<kernel> {
 public:
  /**
   * Constructor
   */
  explicit kernel() = default;

  /**
   * Find on routes
   * @param core
   * @param method
   * @param url
   * @return optional_of<kernel_result>
   */
  static optional_of<kernel_result> find_on_routes(const shared<core>& core,
                                                   const method method,
                                                   const std::string& url) {
    for (const auto& [_route, _controller] : *core->router_->get_routes()) {
      if (auto [_matches, _bindings] = route_match(method, url, _route);
          _matches) {
        return kernel_result{.route_ = _route,
                             .controller_ = _controller,
                             .bindings_ = _bindings};
      }
    }

    return boost::none;
  }

  /**
   * Get available methods
   *
   * @param core
   * @param url
   * @return vector_of<method>
   */
  // LCOV_EXCL_START
  static vector_of<method> get_available_methods(const shared<core>& core,
                                                 const std::string& url) {
    vector_of<method> _methods;
    for (const auto& [_route, _controller] : *core->router_->get_routes()) {
      if (auto [_matches, _bindings] = route_find(url, _route); _matches)
        _methods.push_back(_route.method_);
    }
    return _methods;
  }
  // LCOV_EXCL_STOP

  /**
   * Call
   *
   * @param core
   * @param server_id
   * @param connection
   * @param req
   * @param start_at
   * @param callback
   * @return void
   */
  static void call(const shared<core>& core,
                   uuid server_id,
                   const shared<connection>& connection,
                   const req& req,
                   long start_at,
                   callback_of<boost::beast::http::message_generator> callback);
};

}  // namespace copper::components

#endif
