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

#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/json/parse.hpp>
#include <boost/uuid/random_generator.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/controllers/api/connections_controller.hpp>
#include <copper/components/database.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/http_listener.hpp>
#include <copper/components/req.hpp>
#include <copper/components/res.hpp>
#include <copper/components/router.hpp>
#include <copper/components/signal_handler.hpp>
#include <copper/components/subscriber.hpp>
#include <copper/components/websocket_listener.hpp>
#include <future>
#include <iostream>
#include <thread>

using namespace copper::components;

class WebSocketServerTestFixture : public testing::Test {
 protected:
  void SetUp() override {
    using namespace copper::components;

    std::puts("Generating server ID ...");
    server_id_ = boost::uuids::random_generator()();

    std::puts("Creating address and endpoints ...");
    address_ = boost::asio::ip::make_address("0.0.0.0");
    http_endpoint_ = boost::asio::ip::tcp::endpoint{address_, 9002};
    websocket_endpoint_ = boost::asio::ip::tcp::endpoint{address_, 9003};

    ioc_ = std::make_unique<boost::asio::io_context>(1);

    std::puts("Creating core system ...");
    core_ = core::factory(*ioc_);
    core_->views_->push("404", "404");

    std::puts("Registering HTTP routes ...");
    core_->router_->push(
        method::get, "/api/connections",
        boost::make_shared<controllers::api::connections_controller>(),
        {.use_throttler_ = false, .use_protector_ = false, .rpm_ = 100});

    std::puts("Starting database init ...");
    core_->database_->init(*ioc_, core_);

    std::puts("Starting listeners ...");
    boost::make_shared<copper::components::http_listener>(
        *ioc_, core_, server_id_, http_endpoint_)
        ->run();
    boost::make_shared<copper::components::websocket_listener>(
        *ioc_, core_, server_id_, websocket_endpoint_)
        ->run();

    std::puts("Starting subscriber ...");
    subscriber::start(core_, server_id_);

    thread_ = std::thread([&] { ioc_->run(); });

    std::puts("Waiting for components to become ready ...");
    while (!core_->database_ready_ || !core_->http_ready_ ||
           !core_->websocket_ready_ || !core_->subscriber_ready_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    std::puts("All components ready ...");
  }

  void TearDown() override {
    std::puts("Stopping server components ...");
    post(make_strand(*ioc_), [&]() {
      core_->database_->stop();
      ioc_->stop();
    });

    if (thread_.joinable())
      thread_.join();
  }

  boost::uuids::uuid server_id_;
  boost::asio::ip::address address_;
  boost::asio::ip::tcp::endpoint http_endpoint_;
  boost::asio::ip::tcp::endpoint websocket_endpoint_;
  std::unique_ptr<boost::asio::io_context> ioc_;
  shared<core> core_;
  std::thread thread_;
};

TEST_F(WebSocketServerTestFixture, ConnectsAndCallsApiConnections) {
  boost::asio::io_context _client_ioc;

  // --- WebSocket connection ---
  boost::asio::ip::tcp::resolver _resolver(_client_ioc);
  auto _ws_results = _resolver.resolve("127.0.0.1", "9003");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws(
      make_strand(_client_ioc));
  auto _ep =
      boost::asio::connect(boost::beast::get_lowest_layer(_ws), _ws_results);
  std::string _host = "127.0.0.1:" + std::to_string(_ep.port());

  _ws.handshake(_host, "/");
  _ws.write(boost::asio::buffer("EHLO"));

  boost::beast::flat_buffer _ws_buffer;
  _ws.read(_ws_buffer);  // podrías imprimirlo si querís

  // --- HTTP request ---
  auto _http_results = _resolver.resolve("127.0.0.1", "9002");
  boost::beast::tcp_stream _stream(_client_ioc);
  _stream.connect(_http_results);

  res _response;
  boost::beast::flat_buffer _buffer;
  req _request{method::get, "/api/connections", 11};
  _request.set(fields::host, "127.0.0.1");
  _request.set(fields::user_agent, "Copper");

  boost::beast::http::write(_stream, _request);
  boost::beast::http::read(_stream, _buffer, _response);

  std::cout << _response.body() << std::endl;

  ASSERT_EQ(_response.result_int(), 200);
  ASSERT_TRUE(boost::starts_with(_response.body(), R"({"items":[{)"));

  // --- Cleanup ---
  boost::system::error_code _ec;
  _stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, _ec);
  _stream.close();

  _ws.close(boost::beast::websocket::close_code::normal, _ec);
}

TEST_F(WebSocketServerTestFixture, ConnectsSendsEHLOAndDisconnects) {
  boost::asio::io_context _client_ioc;
  boost::asio::ip::tcp::resolver _resolver(_client_ioc);
  auto _ws_results = _resolver.resolve("127.0.0.1", "9003");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws(
      make_strand(_client_ioc));
  auto _ep =
      boost::asio::connect(boost::beast::get_lowest_layer(_ws), _ws_results);
  std::string _host = "127.0.0.1:" + std::to_string(_ep.port());

  _ws.handshake(_host, "/");
  _ws.write(boost::asio::buffer("EHLO"));

  boost::beast::flat_buffer _buffer;
  _ws.read(_buffer);

  std::string message = boost::beast::buffers_to_string(_buffer.data());
  std::cout << "Received: " << message << std::endl;

  ASSERT_TRUE(_buffer.size() > 0);

  boost::beast::error_code _ec;
  _ws.close(boost::beast::websocket::close_code::normal, _ec);
  ASSERT_FALSE(_ec);
}

TEST_F(WebSocketServerTestFixture, ConnectsSendsEmptyJsonAndDisconnects) {
  boost::asio::io_context _client_ioc;
  boost::asio::ip::tcp::resolver _resolver(_client_ioc);
  auto _ws_results = _resolver.resolve("127.0.0.1", "9003");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws(
      make_strand(_client_ioc));
  auto _ep =
      boost::asio::connect(boost::beast::get_lowest_layer(_ws), _ws_results);
  std::string _host = "127.0.0.1:" + std::to_string(_ep.port());

  _ws.handshake(_host, "/");
  _ws.write(boost::asio::buffer("{}"));

  boost::beast::flat_buffer _buffer;
  _ws.read(_buffer);

  std::string message = boost::beast::buffers_to_string(_buffer.data());
  std::cout << "Received: " << message << std::endl;

  ASSERT_TRUE(_buffer.size() > 0);

  boost::beast::error_code _ec;
  _ws.close(boost::beast::websocket::close_code::normal, _ec);
  ASSERT_FALSE(_ec);
}

TEST_F(WebSocketServerTestFixture, ConnectsSendsAlmostValidCommand) {
  boost::asio::io_context _client_ioc;
  boost::asio::ip::tcp::resolver _resolver(_client_ioc);
  auto _ws_results = _resolver.resolve("127.0.0.1", "9003");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws(
      make_strand(_client_ioc));
  auto _ep =
      boost::asio::connect(boost::beast::get_lowest_layer(_ws), _ws_results);
  std::string _host = "127.0.0.1:" + std::to_string(_ep.port());

  _ws.handshake(_host, "/");

  std::string special_payload =
      R"({"id":"bc7ef697-f39a-4cb6-b9ed-b6d6d42a4357"})";
  _ws.write(boost::asio::buffer(special_payload));

  boost::beast::flat_buffer _buffer;
  _ws.read(_buffer);

  std::string message = boost::beast::buffers_to_string(_buffer.data());
  std::cout << "Received: " << message << std::endl;

  ASSERT_TRUE(_buffer.size() > 0);

  boost::beast::error_code _ec;
  _ws.close(boost::beast::websocket::close_code::normal, _ec);
  ASSERT_FALSE(_ec);
}

TEST_F(WebSocketServerTestFixture, ConnectsSendsWrongSubscribeCommand) {
  boost::asio::io_context _client_ioc;
  boost::asio::ip::tcp::resolver _resolver(_client_ioc);
  auto _ws_results = _resolver.resolve("127.0.0.1", "9003");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws(
      make_strand(_client_ioc));
  auto _ep =
      boost::asio::connect(boost::beast::get_lowest_layer(_ws), _ws_results);
  std::string _host = "127.0.0.1:" + std::to_string(_ep.port());

  _ws.handshake(_host, "/");

  std::string special_payload =
      R"({"id":"bc7ef697-f39a-4cb6-b9ed-b6d6d42a4357","action":"subscribe","channels":"EHLO"})";
  _ws.write(boost::asio::buffer(special_payload));

  boost::beast::flat_buffer _buffer;
  _ws.read(_buffer);

  std::string message = boost::beast::buffers_to_string(_buffer.data());
  std::cout << "Received: " << message << std::endl;

  ASSERT_TRUE(_buffer.size() > 0);

  boost::beast::error_code _ec;
  _ws.close(boost::beast::websocket::close_code::normal, _ec);
  ASSERT_FALSE(_ec);
}

TEST_F(WebSocketServerTestFixture, ConnectsSendsWrongUnsubscribeCommand) {
  boost::asio::io_context _client_ioc;
  boost::asio::ip::tcp::resolver _resolver(_client_ioc);
  auto _ws_results = _resolver.resolve("127.0.0.1", "9003");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws(
      make_strand(_client_ioc));
  auto _ep =
      boost::asio::connect(boost::beast::get_lowest_layer(_ws), _ws_results);
  std::string _host = "127.0.0.1:" + std::to_string(_ep.port());

  _ws.handshake(_host, "/");

  std::string special_payload =
      R"({"id":"bc7ef697-f39a-4cb6-b9ed-b6d6d42a4357","action":"unsubscribe","channels":"EHLO"})";
  _ws.write(boost::asio::buffer(special_payload));

  boost::beast::flat_buffer _buffer;
  _ws.read(_buffer);

  std::string message = boost::beast::buffers_to_string(_buffer.data());
  std::cout << "Received: " << message << std::endl;

  ASSERT_TRUE(_buffer.size() > 0);

  boost::beast::error_code _ec;
  _ws.close(boost::beast::websocket::close_code::normal, _ec);
  ASSERT_FALSE(_ec);
}

TEST_F(WebSocketServerTestFixture, ConnectsSendsWrongBroadcastCommand) {
  boost::asio::io_context _client_ioc;
  boost::asio::ip::tcp::resolver _resolver(_client_ioc);
  auto _ws_results = _resolver.resolve("127.0.0.1", "9003");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws(
      make_strand(_client_ioc));
  auto _ep =
      boost::asio::connect(boost::beast::get_lowest_layer(_ws), _ws_results);
  std::string _host = "127.0.0.1:" + std::to_string(_ep.port());

  _ws.handshake(_host, "/");

  std::string special_payload =
      R"({"id":"bc7ef697-f39a-4cb6-b9ed-b6d6d42a4357","action":"broadcast","channels":"EHLO"})";
  _ws.write(boost::asio::buffer(special_payload));

  boost::beast::flat_buffer _buffer;
  _ws.read(_buffer);

  std::string message = boost::beast::buffers_to_string(_buffer.data());
  std::cout << "Received: " << message << std::endl;

  ASSERT_TRUE(_buffer.size() > 0);

  boost::beast::error_code _ec;
  _ws.close(boost::beast::websocket::close_code::normal, _ec);
  ASSERT_FALSE(_ec);
}

TEST_F(WebSocketServerTestFixture, ConnectsSendsUnsubscribeCommand) {
  boost::asio::io_context _client_ioc;
  boost::asio::ip::tcp::resolver _resolver(_client_ioc);
  auto _ws_results = _resolver.resolve("127.0.0.1", "9003");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws(
      make_strand(_client_ioc));
  auto _ep =
      boost::asio::connect(boost::beast::get_lowest_layer(_ws), _ws_results);
  std::string _host = "127.0.0.1:" + std::to_string(_ep.port());

  _ws.handshake(_host, "/");

  std::string unsubscribe_payload =
      R"({"id":"bc7ef697-f39a-4cb6-b9ed-b6d6d42a4357","action":"unsubscribe","channels":["EHLO"]})";
  _ws.write(boost::asio::buffer(unsubscribe_payload));

  boost::beast::flat_buffer _buffer;
  _ws.read(_buffer);

  std::string message = boost::beast::buffers_to_string(_buffer.data());
  std::cout << "Received: " << message << std::endl;

  ASSERT_TRUE(_buffer.size() > 0);

  boost::beast::error_code _ec;
  _ws.close(boost::beast::websocket::close_code::normal, _ec);
  ASSERT_FALSE(_ec);
}

TEST_F(WebSocketServerTestFixture, ConnectsSendsSubscribeCommand) {
  boost::asio::io_context _client_ioc;
  boost::asio::ip::tcp::resolver _resolver(_client_ioc);
  auto _ws_results = _resolver.resolve("127.0.0.1", "9003");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws(
      make_strand(_client_ioc));
  auto _ep =
      boost::asio::connect(boost::beast::get_lowest_layer(_ws), _ws_results);
  std::string _host = "127.0.0.1:" + std::to_string(_ep.port());

  _ws.handshake(_host, "/");

  std::string subscribe_payload =
      R"({"id":"bc7ef697-f39a-4cb6-b9ed-b6d6d42a4357","action":"subscribe","channels":["EHLO"]})";
  _ws.write(boost::asio::buffer(subscribe_payload));

  boost::beast::flat_buffer _subscribe_buffer;
  _ws.read(_subscribe_buffer);

  std::string subscribe_message =
      boost::beast::buffers_to_string(_subscribe_buffer.data());
  std::cout << "Received: " << subscribe_message << std::endl;

  ASSERT_TRUE(_subscribe_buffer.size() > 0);

  std::string retry_subscribe_payload =
      R"({"id":"bc7ef697-f39a-4cb6-b9ed-b6d6d42a4357","action":"subscribe","channels":["EHLO"]})";
  _ws.write(boost::asio::buffer(retry_subscribe_payload));

  boost::beast::flat_buffer _retry_subscribe_buffer;
  _ws.read(_retry_subscribe_buffer);

  std::string retry_subscribe_message =
      boost::beast::buffers_to_string(_retry_subscribe_buffer.data());
  std::cout << "Received: " << retry_subscribe_message << std::endl;

  ASSERT_TRUE(retry_subscribe_message.size() > 0);

  std::string unsubscribe_payload =
      R"({"id":"bc7ef697-f39a-4cb6-b9ed-b6d6d42a4357","action":"unsubscribe","channels":["EHLO"]})";
  _ws.write(boost::asio::buffer(unsubscribe_payload));

  boost::beast::flat_buffer _unsubscribe_buffer;
  _ws.read(_unsubscribe_buffer);

  std::string unsubscribe_message =
      boost::beast::buffers_to_string(_unsubscribe_buffer.data());
  std::cout << "Received: " << unsubscribe_message << std::endl;

  ASSERT_TRUE(unsubscribe_message.size() > 0);

  std::string retry_unsubscribe_payload =
      R"({"id":"bc7ef697-f39a-4cb6-b9ed-b6d6d42a4357","action":"unsubscribe","channels":["EHLO"]})";
  _ws.write(boost::asio::buffer(retry_unsubscribe_payload));

  boost::beast::flat_buffer _retry_unsubscribe_buffer;
  _ws.read(_retry_unsubscribe_buffer);

  std::string retry_unsubscribe_message =
      boost::beast::buffers_to_string(_retry_unsubscribe_buffer.data());
  std::cout << "Received: " << retry_unsubscribe_message << std::endl;

  ASSERT_TRUE(retry_unsubscribe_message.size() > 0);

  boost::beast::error_code _ec;
  _ws.close(boost::beast::websocket::close_code::normal, _ec);
  ASSERT_FALSE(_ec);
}

TEST_F(WebSocketServerTestFixture, ConnectsSendsBroadcastCommand) {
  boost::asio::io_context _client_ioc;
  boost::asio::ip::tcp::resolver _resolver(_client_ioc);
  auto _ws_results = _resolver.resolve("127.0.0.1", "9003");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws(
      make_strand(_client_ioc));
  auto _ep =
      boost::asio::connect(boost::beast::get_lowest_layer(_ws), _ws_results);
  std::string _host = "127.0.0.1:" + std::to_string(_ep.port());

  _ws.handshake(_host, "/");

  std::string special_payload =
      R"({"id":"bc7ef697-f39a-4cb6-b9ed-b6d6d42a4357","action":"broadcast","channels":["EHLO"],"data":{"attribute":"value"}})";
  _ws.write(boost::asio::buffer(special_payload));

  std::cout << "Connections: " << core_->state_->get_connections().size()
            << std::endl;

  boost::beast::flat_buffer _buffer;
  _ws.read(_buffer);

  std::string message = boost::beast::buffers_to_string(_buffer.data());
  std::cout << "Received: " << message << std::endl;

  ASSERT_TRUE(_buffer.size() > 0);

  boost::beast::error_code _ec;
  _ws.close(boost::beast::websocket::close_code::normal, _ec);
  ASSERT_FALSE(_ec);

  while (core_->state_->get_connections().size() != 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

TEST_F(WebSocketServerTestFixture, BroadcastsBetweenTwoClients) {
  // Separar contexto por cliente
  boost::asio::io_context _ioc_a;
  boost::asio::io_context _ioc_b;

  // Resolvedores independientes
  boost::asio::ip::tcp::resolver _resolver_a(_ioc_a);
  boost::asio::ip::tcp::resolver _resolver_b(_ioc_b);

  auto _ws_results_a = _resolver_a.resolve("127.0.0.1", "9003");
  auto _ws_results_b = _resolver_b.resolve("127.0.0.1", "9003");

  // Cliente A
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws_a(
      make_strand(_ioc_a));
  auto _ep_a = boost::asio::connect(boost::beast::get_lowest_layer(_ws_a),
                                    _ws_results_a);
  std::string _host = "127.0.0.1:" + std::to_string(_ep_a.port());
  _ws_a.handshake(_host, "/");

  // Cliente B
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws_b(
      make_strand(_ioc_b));
  boost::asio::connect(boost::beast::get_lowest_layer(_ws_b), _ws_results_b);
  _ws_b.handshake(_host, "/");

  // Suscripciones
  const std::string uuid = "d08ea8e3-bab0-4004-9fc8-8dfe3c81e62b";
  std::string subscribe_payload =
      R"({"id":")" + uuid +
      R"(","action":"subscribe","channels":["broadcast_test"]})";
  _ws_a.write(boost::asio::buffer(subscribe_payload));
  _ws_b.write(boost::asio::buffer(subscribe_payload));

  boost::beast::flat_buffer _buf_a1, _buf_b1;
  _ws_a.read(_buf_a1);
  _ws_b.read(_buf_b1);

  // Broadcast desde A
  std::string broadcast_payload =
      R"({"id":")" + uuid +
      R"(","action":"broadcast","channels":["broadcast_test"],"data":{"hello":"world"}})";
  _ws_a.write(boost::asio::buffer(broadcast_payload));

  // Espera para asegurar propagación vía Redis
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // Confirmación para A
  boost::beast::flat_buffer _buf_a2;
  _ws_a.read(_buf_a2);
  std::string message_a = boost::beast::buffers_to_string(_buf_a2.data());

  // Recepción por B
  boost::beast::flat_buffer _buf_b2;
  _ws_b.read(_buf_b2);
  std::string message_b = boost::beast::buffers_to_string(_buf_b2.data());

  std::cout << "Broadcast (A sees): " << message_a << std::endl;
  std::cout << "Broadcast (B receives): " << message_b << std::endl;

  ASSERT_TRUE(message_b.find(R"("action":"broadcast")") != std::string::npos);
  ASSERT_TRUE(message_b.find(R"("hello":"world")") != std::string::npos);

  // Cierre ordenado
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  boost::beast::error_code _ec;
  _ws_a.close(boost::beast::websocket::close_code::normal, _ec);
  _ws_b.close(boost::beast::websocket::close_code::normal, _ec);

  while (core_->state_->get_connections().size() != 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

TEST_F(WebSocketServerTestFixture, BroadcastDoesNotReachUnsubscribedClient) {
  boost::asio::io_context _ioc_a, _ioc_b;
  boost::asio::ip::tcp::resolver _resolver_a(_ioc_a), _resolver_b(_ioc_b);
  auto _results_a = _resolver_a.resolve("127.0.0.1", "9003");
  auto _results_b = _resolver_b.resolve("127.0.0.1", "9003");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws_a(
      make_strand(_ioc_a));
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws_b(
      make_strand(_ioc_b));

  auto _ep_a =
      boost::asio::connect(boost::beast::get_lowest_layer(_ws_a), _results_a);
  auto _ep_b =
      boost::asio::connect(boost::beast::get_lowest_layer(_ws_b), _results_b);
  std::string _host_a = "127.0.0.1:" + std::to_string(_ep_a.port());
  std::string _host_b = "127.0.0.1:" + std::to_string(_ep_b.port());

  _ws_a.handshake(_host_a, "/");
  _ws_b.handshake(_host_b, "/");

  std::string uuid = "unsub-test-1";
  std::string sub =
      R"({"id":")" + uuid + R"(","action":"subscribe","channels":["solo_a"]})";
  _ws_a.write(boost::asio::buffer(sub));

  boost::beast::flat_buffer _buf_a_sub;
  _ws_a.read(_buf_a_sub);

  std::string msg =
      R"({"id":")" + uuid +
      R"(","action":"broadcast","channels":["solo_a"],"data":{"ping":"pong"}})";
  _ws_a.write(boost::asio::buffer(msg));

  boost::beast::flat_buffer _buf_ack;
  _ws_a.read(_buf_ack);

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  boost::beast::error_code _ec;
  _ws_a.close(boost::beast::websocket::close_code::normal, _ec);
  _ws_b.close(boost::beast::websocket::close_code::normal, _ec);

  while (core_->state_->get_connections().size() != 0)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

TEST_F(WebSocketServerTestFixture,
       BroadcastStressTest10MessagesRespectingFrame) {
  boost::asio::io_context _ioc_sender, _ioc_receiver;
  boost::asio::ip::tcp::resolver _resolver_sender(_ioc_sender),
      _resolver_receiver(_ioc_receiver);

  auto _results_sender = _resolver_sender.resolve("127.0.0.1", "9003");
  auto _results_receiver = _resolver_receiver.resolve("127.0.0.1", "9003");

  // Cliente emisor
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws_sender(
      make_strand(_ioc_sender));
  auto _ep_sender = boost::asio::connect(
      boost::beast::get_lowest_layer(_ws_sender), _results_sender);
  std::string _host = "127.0.0.1:" + std::to_string(_ep_sender.port());
  _ws_sender.handshake(_host, "/");
  _ws_sender.text(true);

  // Cliente receptor
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws_receiver(
      make_strand(_ioc_receiver));
  boost::asio::connect(boost::beast::get_lowest_layer(_ws_receiver),
                       _results_receiver);
  _ws_receiver.handshake(_host, "/");
  _ws_receiver.text(true);

  // Suscripción
  std::string uuid = to_string(boost::uuids::random_generator()());
  std::string channel = "test_strict";
  std::string subscribe = R"({"id":")" + uuid +
                          R"(","action":"subscribe","channels":[")" + channel +
                          R"("]})";

  _ws_receiver.write(boost::asio::buffer(subscribe));
  boost::beast::flat_buffer _ack_sub;
  _ws_receiver.read(_ack_sub);

  std::string ack_sub = boost::beast::buffers_to_string(_ack_sub.data());

  std::cout << "ACK SUB: " << ack_sub << std::endl;

  // Enviar 10 mensajes, 1 por frame, con ACK intercalado
  for (int i = 0; i < 10; ++i) {
    boost::json::object msg;
    msg["id"] = to_string(boost::uuids::random_generator()());
    msg["action"] = "broadcast";
    msg["channels"] = boost::json::array({"test_strict"});
    msg["data"] = {{"index", i}};

    std::string payload = serialize(msg);

    std::cout << payload << std::endl;

    _ws_sender.write(boost::asio::buffer(payload));

    boost::beast::flat_buffer _ack;
    std::cout << "[INFO] Esperando ACK del emisor (i=" << i << ")..."
              << std::endl;
    _ws_sender.read(_ack);
    std::string ack_data = boost::beast::buffers_to_string(_ack.data());
    std::cout << ack_data << std::endl;
    ASSERT_TRUE(ack_data.find(R"("status":200)") != std::string::npos);
  }

  // Recibir 10 mensajes en el receptor
  int received = 0;
  for (int i = 0; i < 10; ++i) {
    boost::beast::flat_buffer _buf;
    std::cout << "[INFO] Esperando mensaje #" << i << "..." << std::endl;
    _ws_receiver.read(_buf);

    std::string msg = boost::beast::buffers_to_string(_buf.data());
    if (msg.find(R"("action":"broadcast")") != std::string::npos)
      ++received;
  }

  std::cout << "[strict-mode] Received " << received << " messages."
            << std::endl;
  ASSERT_EQ(received, 10);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  boost::beast::error_code _ec;
  _ws_sender.close(boost::beast::websocket::close_code::normal, _ec);
  _ws_receiver.close(boost::beast::websocket::close_code::normal, _ec);

  while (core_->state_->get_connections().size() != 0)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

TEST_F(WebSocketServerTestFixture, BroadcastToMultipleChannels) {
  boost::asio::io_context _ioc_a, _ioc_b;
  boost::asio::ip::tcp::resolver _resolver_a(_ioc_a), _resolver_b(_ioc_b);
  auto _results_a = _resolver_a.resolve("127.0.0.1", "9003");
  auto _results_b = _resolver_b.resolve("127.0.0.1", "9003");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws_a(
      make_strand(_ioc_a));
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws_b(
      make_strand(_ioc_b));

  auto _ep_a =
      boost::asio::connect(boost::beast::get_lowest_layer(_ws_a), _results_a);
  boost::asio::connect(boost::beast::get_lowest_layer(_ws_b), _results_b);
  std::string _host = "127.0.0.1:" + std::to_string(_ep_a.port());

  _ws_a.handshake(_host, "/");
  _ws_b.handshake(_host, "/");

  std::string uuid = "22d21e79-2c79-4ff1-9f35-2f1d15bb4fd0";
  std::string sub_a = R"({"id":")" + uuid +
                      R"(","action":"subscribe","channels":["uno","dos"]})";
  std::string sub_b =
      R"({"id":")" + uuid + R"(","action":"subscribe","channels":["dos"]})";

  _ws_a.write(boost::asio::buffer(sub_a));
  _ws_b.write(boost::asio::buffer(sub_b));

  boost::beast::flat_buffer _buf_a_sub, _buf_b_sub;
  _ws_a.read(_buf_a_sub);
  _ws_b.read(_buf_b_sub);

  std::string msg =
      R"({"id":")" + uuid +
      R"(","action":"broadcast","channels":["uno","dos"],"data":{"z":"w"}})";
  _ws_a.write(boost::asio::buffer(msg));

  boost::beast::flat_buffer _buf_a_ack;
  _ws_a.read(_buf_a_ack);  // A recibe su ACK

  std::this_thread::sleep_for(
      std::chrono::milliseconds(200));  // Redis en difusión

  boost::beast::flat_buffer _buf_b_recv;
  _ws_b.read(_buf_b_recv);  // B debería recibir broadcast

  std::string received = boost::beast::buffers_to_string(_buf_b_recv.data());
  ASSERT_TRUE(received.find(R"("action":"broadcast")") != std::string::npos);
  ASSERT_TRUE(received.find(R"("z":"w")") != std::string::npos);

  boost::beast::error_code _ec;
  _ws_a.close(boost::beast::websocket::close_code::normal, _ec);
  _ws_b.close(boost::beast::websocket::close_code::normal, _ec);

  while (core_->state_->get_connections().size() != 0)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

TEST_F(WebSocketServerTestFixture, UnsubscribedClientDoesNotReceiveBroadcast) {
  boost::asio::io_context _ioc;
  boost::asio::ip::tcp::resolver _resolver(_ioc);
  auto _results = _resolver.resolve("127.0.0.1", "9003");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws(
      make_strand(_ioc));
  auto _ep =
      boost::asio::connect(boost::beast::get_lowest_layer(_ws), _results);
  std::string _host = "127.0.0.1:" + std::to_string(_ep.port());
  _ws.handshake(_host, "/");

  std::string uuid = "59c74032-2240-4591-b404-552486f03016";
  std::string sub =
      R"({"id":")" + uuid + R"(","action":"subscribe","channels":["alpha"]})";
  std::string unsub =
      R"({"id":")" + uuid + R"(","action":"unsubscribe","channels":["alpha"]})";
  std::string msg =
      R"({"id":")" + uuid +
      R"(","action":"broadcast","channels":["alpha"],"data":{"foo":"bar"}})";

  _ws.write(boost::asio::buffer(sub));
  boost::beast::flat_buffer _buf_sub;
  _ws.read(_buf_sub);

  _ws.write(boost::asio::buffer(unsub));
  boost::beast::flat_buffer _buf_unsub;
  _ws.read(_buf_unsub);

  _ws.write(boost::asio::buffer(msg));
  boost::beast::flat_buffer _buf_ack;
  _ws.read(_buf_ack);

  std::string ack_str = boost::beast::buffers_to_string(_buf_ack.data());
  ASSERT_TRUE(ack_str.find(R"("status":200)") != std::string::npos);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  boost::beast::error_code _ec;
  _ws.close(boost::beast::websocket::close_code::normal, _ec);

  while (core_->state_->get_connections().size() != 0)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

TEST_F(WebSocketServerTestFixture, BroadcastAllSendsToAllClients) {
  boost::asio::io_context _ioc1, _ioc2;
  boost::asio::ip::tcp::resolver _resolver1(_ioc1), _resolver2(_ioc2);
  auto _res1 = _resolver1.resolve("127.0.0.1", "9003");
  auto _res2 = _resolver2.resolve("127.0.0.1", "9003");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws1(
      make_strand(_ioc1));
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws2(
      make_strand(_ioc2));

  auto _ep1 = boost::asio::connect(boost::beast::get_lowest_layer(_ws1), _res1);
  boost::asio::connect(boost::beast::get_lowest_layer(_ws2), _res2);
  std::string _host = "127.0.0.1:" + std::to_string(_ep1.port());

  _ws1.handshake(_host, "/");
  _ws2.handshake(_host, "/");

  // Enviar broadcast_all desde ws1
  std::string uuid = to_string(boost::uuids::random_generator()());
  std::string payload =
      R"({"id":")" + uuid +
      R"(","action":"broadcast_all","data":{"global":"ping"}})";
  _ws1.write(boost::asio::buffer(payload));

  // Leer dos mensajes del emisor (ACK + broadcast)
  boost::beast::flat_buffer _buf1a, _buf1b;
  _ws1.read(_buf1a);
  _ws1.read(_buf1b);

  std::string m1a = boost::beast::buffers_to_string(_buf1a.data());
  std::string m1b = boost::beast::buffers_to_string(_buf1b.data());

  std::string ack_data, broadcast_data;
  if (m1a.find(R"("status":200)") != std::string::npos) {
    ack_data = m1a;
    broadcast_data = m1b;
  } else {
    ack_data = m1b;
    broadcast_data = m1a;
  }

  ASSERT_TRUE(ack_data.find(R"("status":200)") != std::string::npos);
  ASSERT_TRUE(broadcast_data.find(R"("action":"broadcast_all")") !=
              std::string::npos);
  ASSERT_TRUE(broadcast_data.find(R"("global":"ping")") != std::string::npos);

  // Leer mensaje del receptor
  boost::beast::flat_buffer _buf2;
  _ws2.read(_buf2);
  std::string msg2 = boost::beast::buffers_to_string(_buf2.data());

  ASSERT_TRUE(msg2.find(R"("action":"broadcast_all")") != std::string::npos);
  ASSERT_TRUE(msg2.find(R"("global":"ping")") != std::string::npos);

  // Cierre limpio
  boost::beast::error_code _ec;
  _ws1.close(boost::beast::websocket::close_code::normal, _ec);
  _ws2.close(boost::beast::websocket::close_code::normal, _ec);

  while (core_->state_->get_connections().size() != 0)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

TEST_F(WebSocketServerTestFixture, BroadcastAllFailsIfDataIsInvalid) {
  boost::asio::io_context _ioc;
  boost::asio::ip::tcp::resolver _resolver(_ioc);
  auto _res = _resolver.resolve("127.0.0.1", "9003");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws(
      make_strand(_ioc));
  auto _ep = boost::asio::connect(boost::beast::get_lowest_layer(_ws), _res);
  std::string _host = "127.0.0.1:" + std::to_string(_ep.port());
  _ws.handshake(_host, "/");

  std::string uuid = to_string(boost::uuids::random_generator()());
  std::string payload =
      R"({"id":")" + uuid +
      R"(","action":"broadcast_all","data":"string_instead_of_object"})";
  _ws.write(boost::asio::buffer(payload));

  boost::beast::flat_buffer _buffer;
  _ws.read(_buffer);

  std::string response = boost::beast::buffers_to_string(_buffer.data());
  std::cout << "Response: " << response << std::endl;

  ASSERT_TRUE(response.find(R"("status":422)") != std::string::npos);
  ASSERT_TRUE(response.find(R"("errors")") != std::string::npos);

  boost::beast::error_code _ec;
  _ws.close(boost::beast::websocket::close_code::normal, _ec);
}