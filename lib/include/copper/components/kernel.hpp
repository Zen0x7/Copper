#ifndef COPPER_COMPONENTS_KERNEL_HPP
#define COPPER_COMPONENTS_KERNEL_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/containers.hpp>
#include <copper/components/normalized_path.hpp>
#include <copper/components/request.hpp>
#include <copper/components/response_generic.hpp>
#include <copper/components/route.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>
#include <copper/models/request.hpp>
#include <copper/models/response.hpp>

#ifndef HTTP_SERVER_HEADER_CONTENT
#define HTTP_SERVER_HEADER_CONTENT "Copper"
#endif

namespace copper::components {

/**
 * Forward state
 */
class state;

/**
 * Forward controller
 */
class controller;

/**
 * Kernel result
 */
struct kernel_result {
  route route_;
  shared<controller> controller_;
  containers::unordered_map_of_strings bindings_;
};

/**
 * Kernel
 */
class kernel : public shared_enabled<kernel> {
 public:
  /**
   * Constructor
   */
  explicit kernel() {}

  /**
   * Find on routes
   * @param method
   * @param url
   * @return optional_of<kernel_result>
   */
  containers::optional_of<kernel_result> find_on_routes(
      method method, const std::string &url) const;

  /**
   * Get available methods
   *
   * @param url
   * @return vector_of<method>
   */
  containers::vector_of<method> get_available_methods(
      const std::string &url) const;

  /**
   * Call
   *
   * @param session_id
   * @param request
   * @param ip
   * @param request_id
   * @param start_at
   * @return async_of<tuple_of<shared<models::request>,
   * shared<models::response>, response_generic>>
   */
  containers::async_of<containers::tuple_of<
      shared<models::request>, shared<models::response>, response_generic>>
  call(uuid session_id, boost::beast::string_view /* root */,
       const request &request, const std::string &ip, const uuid &request_id,
       long start_at) const;
};

}  // namespace copper::components

#endif