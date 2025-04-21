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

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/json/parse.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/redis/connection.hpp>
#include <copper/components/cache.hpp>
#include <copper/components/core.hpp>
#include <copper/components/protocol.hpp>
#include <copper/components/subscriber.hpp>
#include <iostream>

namespace copper::components {
void subscriber::start(const shared<core>& core, const uuid& server_id) {
  core->cache_->get_connection(
      [core, server_id](const shared<boost::redis::connection>& conn) {
        boost::make_shared<subscriber>(core, server_id, conn)->run();
      });
}

subscriber::subscriber(const shared<core>& core,
                       uuid server_id,
                       const shared<boost::redis::connection>& connection)
    : core_(core), server_id_(server_id), connection_(connection) {
  request_.push("SUBSCRIBE", "copper");
}

void subscriber::run() {
  connection_->set_receive_response(response_);
  core_->subscriber_ready_ = true;
  std::puts("Subscriber starting ...");

  executor_ = connection_->get_executor();

  connection_->async_exec(
      request_, boost::redis::ignore,
      bind_executor(executor_, [self = shared_from_this()](
                                   boost::system::error_code ec, std::size_t) {
        // LCOV_EXCL_START
        if (ec) {
          self->error_ = ec;
        }
        // LCOV_EXCL_STOP
        self->on_subscribe(ec);
      }));
}

void subscriber::on_subscribe(boost::system::error_code error) {
  // LCOV_EXCL_START
  if (error) {
    LOG("Subscribe error: " << error.message());
    return;
  }
  // LCOV_EXCL_STOP

  std::puts("Subscriber subscribed. Starting receive loop...");
  do_receive();
}

void subscriber::do_receive() {
  connection_->async_receive(bind_executor(
      executor_,
      [self = shared_from_this()](boost::system::error_code ec, std::size_t) {
        // LCOV_EXCL_START
        if (ec) {
          self->error_ = ec;
        }
        // LCOV_EXCL_STOP
        self->on_receive(ec);
      }));
}

void subscriber::on_receive(boost::system::error_code error) {
  // LCOV_EXCL_START
  if (error == boost::redis::error::sync_receive_push_failed) {
    std::puts("sync_receive_push_failed, retrying...");
    do_receive();
    return;
  }
  if (error) {
    LOG("Receive error: " << error.message());
    return;
  }

  try {
    LOG(response_.value().at(1).value << " " << response_.value().at(2).value
                                      << " " << response_.value().at(3).value);

    boost::redis::consume_one(response_);

    if (response_.value().size() >= 4 &&
        response_.value().at(0).value == "message") {
      const auto json_str = response_.value().at(3).value;
      const auto parsed = boost::json::parse(json_str);
      const auto obj = parsed.as_object();

      if (!obj.contains("server_id") ||
          obj.at("server_id").as_string() != to_string(server_id_)) {
        if (obj.contains("action") &&
            obj.at("action").as_string() == "broadcast") {
          const auto id = obj.at("id").as_string();
          const auto connection_id =
              boost::lexical_cast<uuid>(obj.at("connection_id").as_string());
          const auto channels = obj.at("channels");
          const auto data = obj.at("data");

          auto response = boost::make_shared<protocol_response>();
          protocol_handler_any(core_, server_id_, connection_id, id,
                               "broadcast", parsed, response, false);
        }
      }
    }
  } catch (const std::exception& ex) {
    LOG("[subscriber] error parsing broadcast: " << ex.what());
  }
  // LCOV_EXCL_STOP

  do_receive();  // seguir escuchando
}
}  // namespace copper::components
