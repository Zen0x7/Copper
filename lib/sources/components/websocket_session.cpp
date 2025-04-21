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

#include <copper/components/chronos.hpp>
#include <copper/components/connection.hpp>
#include <copper/components/core.hpp>
#include <copper/components/protocol.hpp>
#include <copper/components/report.hpp>
#include <copper/components/state.hpp>
#include <copper/components/websocket_session.hpp>

#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/websocket.hpp>

namespace copper::components {

websocket_session::websocket_session(const shared<core>& core,
                                     const uuid& server_id,
                                     const shared<connection>& connection)
    : core_(core), server_id_(server_id), connection_(connection) {
  ws_ = &std::get<connection::websocket_stream>(connection_->stream_);
}

websocket_session::~websocket_session() {
#ifdef DEBUG_ENABLED
  core_->database_->connection_closed(core_, connection_->id_,
                                      "The socket was closed");
#endif
  LOG("[websocket_session] disconnected");
  core_->state_->disconnected(connection_->id_);
  connection_->shutdown();
}

void websocket_session::start() {
  auto& _socket = boost::beast::get_lowest_layer(*ws_);
  _socket.expires_never();
  // LCOV_EXCL_START
  ws_->set_option(boost::beast::websocket::stream_base::timeout::suggested(
      boost::beast::role_type::server));

  ws_->set_option(boost::beast::websocket::stream_base::decorator(
      [](boost::beast::websocket::response_type& res) {
        res.set(boost::beast::http::field::server, "Copper");
      }));
  // LCOV_EXCL_STOP

  ws_->async_accept(boost::beast::bind_front_handler(
      &websocket_session::on_accept, shared_from_this()));
}

void websocket_session::on_accept(boost::system::error_code error) {
  // LCOV_EXCL_START
  if (error)
    return report(error, "accept");
  // LCOV_EXCL_STOP

  do_read();
}

void websocket_session::do_read() {
  ws_->async_read(buffer_,
                  boost::beast::bind_front_handler(&websocket_session::on_read,
                                                   shared_from_this()));
}

void websocket_session::on_read(boost::system::error_code error,
                                std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (error == boost::beast::websocket::error::closed)
    return;

  // LCOV_EXCL_START
  if (error)
    return report(error, "read");
  // LCOV_EXCL_STOP

  const auto response =
      protocol_handler_from_websocket(core_, server_id_, connection_, buffer_);

  const auto data = serialize(response->data_);

  ws_->text(true);
  connection_->send(boost::make_shared<const std::string>(std::move(data)));
  buffer_.consume(buffer_.size());

  do_read();
}
}  // namespace copper::components