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

#include <gtest/gtest.h>

#include <boost/algorithm/hex.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/uuid/random_generator.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/controllers/api/connections_controller.hpp>
#include <copper/components/database.hpp>
#include <copper/components/http_listener.hpp>
#include <copper/components/router.hpp>
#include <copper/components/signal_handler.hpp>
#include <copper/components/tcp_listener.hpp>
#include <future>
#include <iostream>
#include <thread>

class TcpServerTestFixture : public testing::Test {
 protected:
  void SetUp() override {
    std::puts("Generating server ID ...");
    server_id_ = boost::uuids::random_generator()();

    std::puts("Creating TCP endpoint ...");
    address_ = boost::asio::ip::make_address("0.0.0.0");
    tcp_endpoint_ = boost::asio::ip::tcp::endpoint{address_, 9005};

    std::puts("Initializing io_context ...");
    ioc_ = std::make_unique<boost::asio::io_context>(1);

    std::puts("Starting core factory ...");
    core_ = copper::components::core::factory(*ioc_);
    std::puts("Core factory initialized ...");

    std::puts("Spawning database init ...");
    core_->database_->init(*ioc_, core_);

    std::puts("Spawning TCP listener ...");
    boost::make_shared<copper::components::tcp_listener>(
        *ioc_, core_, server_id_, tcp_endpoint_)
        ->run();
    std::puts("TCP listener spawned ...");

    std::puts("Launching IO thread ...");
    thread_ = std::thread([&] { ioc_->run(); });

    std::puts("Waiting for readiness flags ...");
    while (!core_->database_ready_ || !core_->tcp_ready_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
  }

  void TearDown() override {
    std::puts("Flushing and stopping server DB and IO ...");
    post(make_strand(*ioc_), [&]() {
      std::puts("Stopping database ...");
      core_->database_->stop();
      std::puts("Database stopped. Stopping io_context ...");
      ioc_->stop();
    });

    if (thread_.joinable()) {
      thread_.join();
    }

    std::puts("All server threads joined. Test finished.");
  }

  std::string HexToBinaryString(const std::string& hex_string) {
    std::vector<unsigned char> binary_data;
    boost::algorithm::unhex(hex_string.begin(), hex_string.end(),
                            std::back_inserter(binary_data));
    return std::string(binary_data.begin(), binary_data.end());
  }

  boost::uuids::uuid server_id_;
  boost::asio::ip::address address_;
  boost::asio::ip::tcp::endpoint tcp_endpoint_;
  copper::components::shared<boost::asio::io_context> ioc_;
  copper::components::shared<copper::components::core> core_;
  std::thread thread_;
};

TEST_F(TcpServerTestFixture, SendsBinaryAndReceivesResponse) {
  boost::asio::io_context client_ioc_;
  std::puts("Creating resolver ...");
  boost::asio::ip::tcp::resolver resolver_(client_ioc_);
  std::puts("Resolver created ...");

  std::string host_ = "127.0.0.1";
  std::puts("Resolving TCP service ...");
  auto const tcp_results_ = resolver_.resolve(host_, "9005");
  std::puts("Service resolved ...");

  std::puts("Connecting TCP stream ...");
  boost::beast::tcp_stream stream_(make_strand(client_ioc_));
  stream_.connect(tcp_results_);
  std::puts("Stream connected ...");

  std::puts("Building first message from hex ...");
  std::string welcome_message_hex_ = "000F383633373139303634303333343638";
  std::string welcome_message_ = HexToBinaryString(welcome_message_hex_);

  std::puts(welcome_message_.data());

  std::puts("Sending first message (welcome) ...");
  boost::asio::write(stream_.socket(), boost::asio::buffer(welcome_message_));
  std::puts("First message sent ...");

  std::puts("Reading server response ...");
  boost::beast::flat_buffer buffer_;
  std::size_t bytes_ = stream_.socket().read_some(buffer_.prepare(1024));
  buffer_.commit(bytes_);
  std::puts("Response received ...");

  std::string response_(buffers_begin(buffer_.data()),
                        buffers_begin(buffer_.data()) + buffer_.size());
  std::puts("Server response:");
  std::puts(response_.c_str());

  ASSERT_FALSE(response_.empty());

  std::puts("Building second message from hex ...");
  std::string second_message_hex_ =
      "00000000000000728E0100000196034A09D000D54EDA27EC08E603"
      "003501420F00000000"
      "0014000B00EF0100F00000150500C80000450100010000B3000002"
      "0000030000B400017C"
      "00000700B5000A00B6000600424BC000430000004400000009002B"
      "0006002B000200F100"
      "011D2900100000000000000000010000A1DA";
  std::string second_message_ = HexToBinaryString(second_message_hex_);

  std::puts("Sending second message ...");
  boost::asio::write(stream_.socket(), boost::asio::buffer(second_message_));
  std::puts("Second message sent ...");

  std::puts("Building and sending third message (0xFF) ...");
  std::string third_message_(1, static_cast<char>(0xFF));
  boost::asio::write(stream_.socket(), boost::asio::buffer(third_message_));
  std::puts("Third message sent (0xFF) ...");

  std::puts("Closing TCP socket ...");
  boost::beast::error_code ec_;
  stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec_);
  if (ec_ && ec_ != boost::system::errc::not_connected) {
    std::puts("Error while closing socket:");
    std::puts(ec_.message().c_str());
  } else {
    std::puts("TCP socket closed cleanly ...");
  }

  std::puts("Running remaining client tasks ...");
  client_ioc_.run();

  ASSERT_TRUE(true);
}