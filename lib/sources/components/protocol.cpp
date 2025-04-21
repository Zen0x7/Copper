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

#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/json/parse.hpp>
#include <boost/lexical_cast.hpp>
#include <copper/components/protocol.hpp>
#include <copper/components/state.hpp>
#include <copper/components/status_code.hpp>
#include <copper/components/validator.hpp>
#include <iostream>

namespace copper::components {
shared<protocol_response> protocol_handler_from_websocket(
    const shared<core>& core,
    const uuid server_id,
    const shared<connection>& connection,
    boost::beast::flat_buffer& buffer) {
  auto _response = boost::make_shared<protocol_response>();
  std::string _data = boost::beast::buffers_to_string(buffer.data());
  boost::system::error_code _ec;
  const auto _value = boost::json::parse(_data, _ec);

  if (_ec) {
    std::puts("Something went wrong [protocol.cpp]");
    std::puts(_ec.what().c_str());
    _response->data_ = {
        {"action", "ack"},
        {"message", "Something went wrong"},
        {"errors", {"*", "Payload must be a valid JSON object"}},
        {"status", static_cast<int>(status_code::unprocessable_entity)}};
  } else {
    map_of_strings _rules = {
        {"*", "is_object"}, {"id", "is_uuid"}, {"action", "is_string"}};

    const auto _validator = validator_make(_rules, _value);

    if (_validator->success_) {
      const auto& _object = _value.as_object();
      const auto _id = _object.at("id").as_string();
      const auto _action = _object.at("action").as_string();

      return protocol_handler_any(core, server_id, connection->id_, _id,
                                  _action, _value, _response, true);
    }

    _response->data_ = {
        {"action", "ack"},
        {"message", "Something went wrong"},
        {"errors", _validator->errors_},
        {"status", static_cast<int>(status_code::unprocessable_entity)}};
  }

  return _response;
}

shared<protocol_response> protocol_handler_any(
    const shared<core>& core,
    const uuid server_id,
    uuid connection_id,
    json::string id,
    const json::string& action,
    json::value value,
    shared<protocol_response> response,
    bool from_websockets = false) {
  if (action == "subscribe") {
    const map_of_strings _rules = {{"channels", "is_array_of_strings"}};

    if (const auto _validator = validator_make(_rules, value);
        _validator->success_) {
      const auto _channels = value.as_object().at("channels").as_array();

      if (!from_websockets) {
        // Signfiica que proviene desde el subscriber ...
      } else {
        boost::json::array _accepted_channels;
        boost::json::array _rejected_channels;
        for (auto _channel : _channels) {
          auto _channel_name = std::string(_channel.as_string());

          if (!core->state_->is_subscribed(connection_id, _channel_name)) {
            core->state_->subscribe(connection_id, _channel_name);
            _accepted_channels.emplace_back(_channel_name);
          } else {
            _rejected_channels.emplace_back(_channel_name);
          }
        }
        response->data_ = {{"action", "ack"},
                           {"id", id},
                           {"message", "Success"},
                           {"data",
                            {{"accepted", _accepted_channels},
                             {"rejected", _rejected_channels}}},
                           {"status", static_cast<int>(status_code::ok)}};
      }
    } else {
      response->data_ = {
          {"action", "ack"},
          {"id", id},
          {"message", "Something went wrong"},
          {"errors", _validator->errors_},
          {"status", static_cast<int>(status_code::unprocessable_entity)}};
    }
  } else if (action == "unsubscribe") {
    const map_of_strings _rules = {{"channels", "is_array_of_strings"}};

    if (const auto _validator = validator_make(_rules, value);
        _validator->success_) {
      const auto _channels = value.as_object().at("channels").as_array();

      if (!from_websockets) {
        // Significa que proviene desde el suscriber ...
      } else {
        boost::json::array _accepted_channels;
        boost::json::array _rejected_channels;
        for (auto _channel : _channels) {
          auto _channel_name = std::string(_channel.as_string());

          if (core->state_->is_subscribed(connection_id, _channel_name)) {
            core->state_->unsubscribe(connection_id, _channel_name);
            _accepted_channels.emplace_back(_channel_name);
          } else {
            _rejected_channels.emplace_back(_channel_name);
          }
        }
        response->data_ = {{"action", "ack"},
                           {"id", id},
                           {"message", "Success"},
                           {"data",
                            {{"accepted", _accepted_channels},
                             {"rejected", _rejected_channels}}},
                           {"status", static_cast<int>(status_code::ok)}};
      }
    } else {
      response->data_ = {
          {"action", "ack"},
          {"id", id},
          {"message", "Something went wrong"},
          {"errors", _validator->errors_},
          {"status", static_cast<int>(status_code::unprocessable_entity)}};
    }
  } else if (action == "broadcast") {
    const map_of_strings _rules = {{"channels", "is_array_of_strings"},
                                   {"data", "is_object"}};

    if (const auto _validator = validator_make(_rules, value);
        _validator->success_) {
      const auto _channels = value.as_object().at("channels").as_array();
      const auto& _data = value.as_object().at("data");
      const std::string _message = serialize(
          boost::json::object({{"action", "broadcast"},
                               {"server_id", to_string(server_id)},
                               {"connection_id", to_string(connection_id)},
                               {"id", id},
                               {"channels", _channels},
                               {"data", _data}}));

      if (from_websockets) {
        vector_of<std::string> _channel_names;
        for (const auto& c : _channels)
          _channel_names.emplace_back(c.as_string());

        core->state_->broadcast(_channel_names, _message);

        core->cache_->get_connection(
            [_message](const shared<boost::redis::connection>& connection) {
              auto req = boost::make_shared<boost::redis::request>();
              req->push("PUBLISH", "copper", _message);
              connection->async_exec(
                  *req, boost::redis::ignore,
                  [req](boost::system::error_code error, std::size_t length) {
                    boost::ignore_unused(error, length);
                    // LCOV_EXCL_START
                    if (error) {
                      LOG("[protocol_handler_any] error on broadcast publish: "
                          << error.what());
                    }
                    // LCOV_EXCL_STOP
                  });
            });
      } else {
        // LCOV_EXCL_START
        vector_of<std::string> _channel_names;
        for (const auto& c : _channels)
          _channel_names.emplace_back(c.as_string());

        core->state_->broadcast(_channel_names, _message);
        // LCOV_EXCL_STOP
      }

      response->data_ = {{"action", "ack"},
                         {"id", id},
                         {"message", "Success"},
                         {"status", static_cast<int>(status_code::ok)}};
    } else {
      response->data_ = {
          {"action", "ack"},
          {"id", id},
          {"message", "Something went wrong"},
          {"errors", _validator->errors_},
          {"status", static_cast<int>(status_code::unprocessable_entity)}};
    }
  } else if (action == "broadcast_all") {
    const map_of_strings _rules = {{"data", "is_object"}};

    if (const auto _validator = validator_make(_rules, value);
        _validator->success_) {
      const auto& _data = value.as_object().at("data");
      const std::string _message = serialize(
          boost::json::object({{"action", "broadcast_all"},
                               {"server_id", to_string(server_id)},
                               {"connection_id", to_string(connection_id)},
                               {"id", id},
                               {"data", _data}}));

      if (from_websockets) {
        core->state_->broadcast_all(_message);

        core->cache_->get_connection([_message](
                                         const shared<boost::redis::connection>&
                                             connection) {
          auto req = boost::make_shared<boost::redis::request>();
          req->push("PUBLISH", "copper", _message);
          connection->async_exec(
              *req, boost::redis::ignore,
              [req](boost::system::error_code error, std::size_t length) {
                boost::ignore_unused(error, length);
                // LCOV_EXCL_START
                if (error) {
                  LOG("[protocol_handler_any] error on broadcast_all publish: "
                      << error.what());
                }
                // LCOV_EXCL_STOP
              });
        });
      } else {
        // LCOV_EXCL_START
        core->state_->broadcast_all(_message);
        // LCOV_EXCL_STOP
      }

      response->data_ = {{"action", "ack"},
                         {"id", id},
                         {"message", "Success"},
                         {"status", static_cast<int>(status_code::ok)}};
    } else {
      response->data_ = {
          {"action", "ack"},
          {"id", id},
          {"message", "Something went wrong"},
          {"errors", _validator->errors_},
          {"status", static_cast<int>(status_code::unprocessable_entity)}};
    }
  }

  return response;
}
}  // namespace copper::components
