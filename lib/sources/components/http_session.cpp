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

#include <copper/components/http_session.hpp>

#include <copper/components/chronos.hpp>
#include <copper/components/connection.hpp>
#include <copper/components/core.hpp>
#include <copper/components/kernel.hpp>
#include <copper/components/report.hpp>
#include <copper/components/state.hpp>

#include <boost/beast/core/bind_handler.hpp>

namespace copper::components {
http_session::http_session(const shared<core>& core,
                           const uuid& server_id,
                           const shared<connection>& connection)
    : core_(core), server_id_(server_id), connection_(connection) {
  stream_ = &std::get<connection::tcp_stream>(connection_->stream_);
  socket_ = &get_lowest_layer(*stream_);
}

http_session::~http_session() {
#ifdef DEBUG_ENABLED
  core_->database_->connection_closed(core_, connection_->id_,
                                      "The socket was closed");
#endif
  LOG("[http_session] disconnected");
  core_->state_->disconnected(connection_->id_);
  connection_->shutdown();
}

void http_session::start() {
  LOG("[http_session@start] scope_in");
  dispatch(stream_->get_executor(),
           boost::beast::bind_front_handler(&http_session::do_read,
                                            shared_from_this()));
  LOG("[http_session@start] scope_out");
}

void http_session::do_read() {
  LOG("[http_session@do_read] scope_in");
  req_ = {};

  stream_->expires_after(std::chrono::seconds(30));

  boost::beast::http::async_read(
      *stream_, buffer_, req_,
      boost::beast::bind_front_handler(&http_session::on_read,
                                       shared_from_this()));
  LOG("[http_session@do_read] scope_out");
}

void http_session::on_read(boost::beast::error_code error,
                           std::size_t bytes_transferred) {
  LOG("[http_session@on_read] scope_in");
  boost::ignore_unused(bytes_transferred);

  if (error == boost::beast::http::error::end_of_stream)
    return do_close();

  // LCOV_EXCL_START
  if (error)
    return report(error, "read");
  // LCOV_EXCL_STOP

  auto now = chronos::now();

  const auto _kernel = boost::make_shared<kernel>();

  _kernel->call(core_, server_id_, connection_, req_, now,
                [self = shared_from_this()](
                    boost::beast::http::message_generator response) {
                  self->send_response(std::move(response));
                });
  LOG("[http_session@on_read] scope_out");
}

void http_session::send_response(boost::beast::http::message_generator&& msg) {
  bool keep_alive = msg.keep_alive();

  boost::beast::async_write(
      *stream_, std::move(msg),
      boost::beast::bind_front_handler(&http_session::on_write,
                                       shared_from_this(), keep_alive));
}
void http_session::on_write(bool keep_alive,
                            boost::beast::error_code ec,
                            std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  // LCOV_EXCL_START
  if (ec)
    return report(ec, "write");

  if (!keep_alive) {
    return do_close();
  }
  // LCOV_EXCL_STOP

  do_read();
}
void http_session::do_close() const {
  boost::beast::error_code ec;
  stream_->socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
}
}  // namespace copper::components
