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

#include <copper/components/core.hpp>
#include <copper/components/database.hpp>
#include <copper/components/report.hpp>
#include <copper/components/state.hpp>
#include <copper/components/tcp_listener.hpp>
#include <copper/components/tcp_session.hpp>

namespace copper::components {

tcp_listener::tcp_listener(boost::asio::io_context& io_context,
                           const shared<core>& core,
                           const uuid& server_id,
                           const boost::asio::ip::tcp::endpoint& endpoint)
    : server_id_(server_id),
      core_(core),
      ioc_(io_context),
      acceptor_(make_strand(io_context)) {
  boost::beast::error_code ec;

  // LCOV_EXCL_START
  acceptor_.open(endpoint.protocol(), ec);
  if (ec) {
    report(ec, "open");
    return;
  }

  acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec) {
    report(ec, "set_option");
    return;
  }

  acceptor_.bind(endpoint, ec);
  if (ec) {
    report(ec, "bind");
    return;
  }

  acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
  if (ec) {
    report(ec, "listen");
    return;
  }
  // LCOV_EXCL_STOP

  core_->tcp_ready_ = true;
}

void tcp_listener::run() {
  do_accept();
}

void tcp_listener::do_accept() {
  acceptor_.async_accept(
      make_strand(ioc_),
      [self = shared_from_this()](const boost::system::error_code& error,
                                  boost::asio::ip::tcp::socket socket) {
        if (!error) {
          auto _connection = boost::make_shared<connection>(std::move(socket));
#ifdef DEBUG_ENABLED
          self->core_->database_->create_connection(self->core_, _connection);
#endif
          self->core_->state_->connected(_connection);
          boost::make_shared<tcp_session>(self->core_, self->server_id_,
                                          _connection)
              ->start();
        }

        self->do_accept();
      });
}
}  // namespace copper::components
