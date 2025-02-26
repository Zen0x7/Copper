#include <gtest/gtest.h>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/ssl.hpp>
#include <copper/components/client_certificates.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/router.hpp>
#include <copper/components/server_certificates.hpp>
#include <copper/components/signal_handler.hpp>
#include <copper/components/state.hpp>
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
    auto _configuration = boost::make_shared<configuration>();

    auto const _address = boost::asio::ip::make_address("0.0.0.0");
    constexpr auto _port = 9002;
    auto const _endpoint = boost::asio::ip::tcp::endpoint{_address, _port};
    constexpr auto _doc_root = std::string_view{"."};
    constexpr auto _threads = 4;

    boost::asio::io_context _ioc{_threads};

    ssl::context _ctx{ssl::context::tlsv12};
    ssl::context _client_ctx{ssl::context::tlsv12};

    load_server_certificate(_ctx);
    load_root_certificates(_client_ctx);

    auto _task_group = boost::make_shared<task_group>(_ioc.get_executor());

    boost::mysql::pool_params _database_params;
    _database_params.server_address.emplace_host_and_port(
        _configuration->get()->database_host_,
        _configuration->get()->database_port_);

    _database_params.username = _configuration->get()->database_user_;
    _database_params.password = _configuration->get()->database_password_;
    _database_params.database = _configuration->get()->database_name_;
    _database_params.thread_safe =
        _configuration->get()->database_pool_thread_safe_;
    _database_params.initial_size =
        _configuration->get()->database_pool_initial_size_;
    _database_params.max_size = _configuration->get()->database_pool_max_size_;

    auto _database_pool = boost::make_shared<boost::mysql::connection_pool>(
        _ioc, std::move(_database_params));

    auto _state = boost::make_shared<state>(_configuration, _database_pool);

    _state->get_database()->start();

    co_spawn(
        make_strand(_ioc),
        listener(_server_id, _state, _task_group, _ctx, _endpoint, _doc_root),
        _task_group->adapt([](const std::exception_ptr &e) {
          if (e) {
            try {
              std::rethrow_exception(e);
            } catch (std::exception &exception) {
              std::cout << "Something went wrong... " << exception.what() << std::endl;
            }
          }
        }));

    co_spawn(make_strand(_ioc), subscriber(_state),
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

    sleep(1);

    boost::asio::ip::tcp::resolver _resolver(_client_ioc);

    std::string _host = "127.0.0.1";
    auto const _results = _resolver.resolve(_host, "9002");

    boost::beast::websocket::stream<ssl::stream<boost::asio::ip::tcp::socket>>
        _ws(_client_ioc, _client_ctx);

    auto _ep =
        boost::asio::connect(boost::beast::get_lowest_layer(_ws), _results);

    if (!SSL_set_tlsext_host_name(_ws.next_layer().native_handle(),
                                  _host.c_str()))
      throw boost::beast::system_error(
          boost::beast::error_code(static_cast<int>(ERR_get_error()),
                                   boost::asio::error::get_ssl_category()),
          "Failed to set SNI Hostname");

    _host += ':' + std::to_string(_ep.port());

    _ws.next_layer().handshake(ssl::stream_base::client);

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

    sleep(5);

    co_spawn(make_strand(_ioc), cancel_websocket_session(),
             boost::asio::detached);
    _ioc.stop();

    sleep(5);

    for (auto &t : _v) t.join();

    ASSERT_TRUE(true);

  } catch (std::runtime_error &e) {
    std::cout << e.what() << std::endl;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  } catch (...) {
    std::cout << "Something went wrong" << std::endl;
  }
}