//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/uuid/random_generator.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/database.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/router.hpp>
#include <copper/components/signal_handler.hpp>
#include <copper/components/subscriber.hpp>
#include <copper/components/task_group.hpp>
#include <iostream>

copper::components::containers::async_of<void> cancel_websocket_session() {
  const auto executor = co_await boost::asio::this_coro::executor;
  executor.get_inner_executor().context().stop();
}

TEST(Components_WebSocket_Session, Implementation) {
  try {
    using namespace copper::components;

    auto _server_id = boost::uuids::random_generator()();
    auto _configuration = configuration::instance();

    auto const _address = boost::asio::ip::make_address("0.0.0.0");
    constexpr auto _port = 9002;
    auto const _endpoint = boost::asio::ip::tcp::endpoint{_address, _port};
    constexpr auto _doc_root = std::string_view{"."};
    constexpr auto _threads = 4;

    boost::asio::io_context _ioc{_threads};

    auto _task_group = boost::make_shared<task_group>(_ioc.get_executor());

    database::setup(_ioc);

    co_spawn(make_strand(_ioc),
             listener(_server_id, _task_group, _endpoint, _doc_root),
             _task_group->adapt([](const std::exception_ptr &e) {
               if (e) {
                 try {
                   std::rethrow_exception(e);
                 } catch (std::exception &exception) {
                   std::cout << "Something went wrong... " << exception.what()
                             << std::endl;
                 }
               }
             }));

    co_spawn(make_strand(_ioc), subscriber(),
             _task_group->adapt([](const std::exception_ptr &e) {
               if (e) {
                 try {
                   std::rethrow_exception(e);
                 } catch (std::exception &exception) {
                   std::cout << "Something went wrong... " << exception.what()
                             << std::endl;
                 }
               }
             }));

    co_spawn(make_strand(_ioc), signal_handler(_task_group),
             boost::asio::detached);

    boost::asio::io_context _client_ioc;

    std::vector<std::thread> _v;
    _v.reserve(_threads);
    for (auto i = _threads; i > 0; --i)
      _v.emplace_back([&_ioc] { _ioc.run(); });

    boost::asio::ip::tcp::resolver _resolver(_client_ioc);

    std::string _host = "127.0.0.1";
    auto const _results = _resolver.resolve(_host, "9002");

    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws(
        _client_ioc);

    auto _ep =
        boost::asio::connect(boost::beast::get_lowest_layer(_ws), _results);

    _host += ':' + std::to_string(_ep.port());

    _ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(
        boost::beast::role_type::client));

    _ws.set_option(boost::beast::websocket::stream_base::decorator(
        [](boost::beast::websocket::request_type &req) {
          req.set(boost::beast::http::field::user_agent, "Copper");
        }));

    _ws.handshake(_host, "/");
    _ws.write(boost::asio::buffer(std::string("hello")));
    boost::beast::flat_buffer _buffer;
    _ws.read(_buffer);

    boost::beast::error_code _ec;
    _ws.close(boost::beast::websocket::close_code::normal, _ec);
    if (_ec) {
    }

    _client_ioc.run();

    co_spawn(make_strand(_ioc), cancel_websocket_session(),
             boost::asio::detached);
    _ioc.stop();

    for (auto &t : _v) t.join();

    ASSERT_TRUE(true);

    database::instance_.reset();

  } catch (std::runtime_error &e) {
    std::cout << e.what() << std::endl;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  } catch (...) {
    std::cout << "Something went wrong" << std::endl;
  }
}