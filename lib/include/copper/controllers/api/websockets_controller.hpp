#ifndef COPPER_CONTROLLERS_API_WEBSOCKETS_CONTROLLER_HPP
#define COPPER_CONTROLLERS_API_WEBSOCKETS_CONTROLLER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/controller.hpp>
#include <copper/components/json.hpp>
#include <copper/components/state.hpp>
#include <copper/components/websocket.hpp>

namespace copper::controllers::api {

/**
 * Websockets controller
 */
class websockets_controller final : public components::controller {
 public:
  /**
   * Invoke
   *
   * @param parameters
   * @return async_of<response>
   */
  components::containers::async_of<components::response> invoke(
      const components::shared<components::controller_parameters> parameters)
      override {
    components::json::array _items;

    for (auto& [_id, _websocket] :
         components::state::instance()->get_websockets()) {
      _items.push_back(components::json::object{
          {"id", to_string(_id)},
          {"session_id", to_string(_websocket->get()->session_id_)},
      });
    }

    boost::json::object _data = {{"items", _items}};

    co_return make_response(parameters, components::status_code::ok,
                            serialize(_data), "application/json");
  }
};

}  // namespace copper::controllers::api

#endif