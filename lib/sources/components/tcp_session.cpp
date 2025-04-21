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
#include <copper/components/state.hpp>
#include <copper/components/tcp_session.hpp>

#include <boost/beast/core/buffers_to_string.hpp>
#include <copper/components/codec.hpp>
#include <copper/components/containers.hpp>

#ifdef DEBUG_ENABLED
#include <boost/algorithm/hex.hpp>
#endif

namespace copper::components {

using namespace containers;

tcp_session::tcp_session(const shared<core>& core,
                         const uuid& server_id,
                         const shared<connection>& connection)
    : core_(core), server_id_(server_id), connection_(connection) {
  stream_ = &std::get<connection::tcp_stream>(connection_->stream_);
  socket_ = &get_lowest_layer(*stream_);
}

tcp_session::~tcp_session() {
#ifdef DEBUG_ENABLED
  core_->database_->connection_closed(core_, connection_->id_,
                                      "The socket was closed");
#endif
  LOG("[tcp_session] disconnected");
  core_->state_->disconnected(connection_->id_);
  connection_->shutdown();
}

void tcp_session::do_welcome() {
  auto self(shared_from_this());
  socket_->socket().async_read_some(
      boost::asio::buffer(data_, 17),
      [this, self](const boost::system::error_code& error,
                   const std::size_t length) mutable {
        if (!error) {
          const auto _imei = codec::codec_get_imei(
              length, reinterpret_cast<unsigned char*>(data_));

          // 000F383633373139303634303333343638

          std::string hex;
          boost::algorithm::hex(data_, data_ + length, std::back_inserter(hex));
          LOG("WELCOME: " << hex);

          core_->database_->find_tracker(
              core_, _imei,
              [this, self](const optional_of<shared<tracker>>& tracker) {
                data_[0] = {0x00};
                // LCOV_EXCL_START
                if (tracker.has_value()) {
                  data_[0] = {0x01};
                }
                // LCOV_EXCL_STOP
                do_welcome_ack(data_[0] == 0x00);
              });
        }
      });
}

void tcp_session::do_welcome_ack(bool closes) {
  auto self(shared_from_this());
  async_write(socket_->socket(), boost::asio::buffer(data_, 1),
              [this, self, closes](const boost::system::error_code& error,
                                   std::size_t /*length*/) {
                if (!error && !closes) {
                  do_read();
                }
              });
}

void tcp_session::do_read() {
  auto self(shared_from_this());
  // LCOV_EXCL_START
  socket_->socket().async_read_some(
      boost::asio::buffer(data_, max_length),
      [this, self](const boost::system::error_code& error, std::size_t length) {
        if (!error) {
          LOG("EVENT: " << boost::algorithm::hex(
                  boost::beast::buffers_to_string(
                      boost::asio::buffer(data_, length))));

          do_read();
        }
      });
  // LCOV_EXCL_STOP
}
}  // namespace copper::components
