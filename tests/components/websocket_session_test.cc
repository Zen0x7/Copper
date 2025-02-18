#include <gtest/gtest.h>

#include <copper/components/certificates.hpp>
#include <copper/components/task_group.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/state.hpp>
#include <copper/components/signal_handler.hpp>

#include <boost/asio/co_spawn.hpp>

boost::asio::awaitable<
  void,
  boost::asio::strand<
    boost::asio::io_context::executor_type
  >
> cancel_websocket_session(boost::asio::io_context &ioc) {
  co_await boost::asio::this_coro::executor;
  boost::asio::post(ioc, [&]() {
    sleep(10);
    ioc.stop();
  });
}

TEST(Components_WebSocket_Session, Implementation) {
  dotenv::init();
  using namespace copper::components;

  auto const address = boost::asio::ip::make_address("0.0.0.0");
  auto const port = 9002;
  auto const endpoint = boost::asio::ip::tcp::endpoint{address, port};
  auto const doc_root = std::string_view{"."};

  boost::asio::io_context ioc;

  boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};

  load_server_certificate(ctx);

  auto task_group_ = boost::make_shared<task_group>(ioc.get_executor());

  auto state_ = boost::make_shared<state>();

  state_->get_database()->start();

  boost::asio::co_spawn(
    boost::asio::make_strand(ioc),
    listener(state_, task_group_, ctx, endpoint, doc_root),
    task_group_->adapt(
      [](std::exception_ptr e) {
        if (e) {
          try {
            std::rethrow_exception(e);
          }
          catch (std::exception &e) {
            std::cerr << "Error in listener: " << e.what() << "\n";
          }
        }
      }));

  boost::asio::co_spawn(boost::asio::make_strand(ioc), signal_handler(task_group_), boost::asio::detached);

  boost::asio::io_context client_ioc;

  std::thread thread([&]() {
    try {
      std::cout << "Running thread #1" << std::endl;
      ioc.run();
      std::cout << "Stopped thread #1" << std::endl;
    } catch (std::exception &e) {

      std::cout << "Exception on thread #1" << std::endl;
    }
  });

  boost::asio::ip::tcp::resolver resolver(client_ioc);

  std::string host = "127.0.0.1";
  auto const results = resolver.resolve(host, "9002");

  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws(client_ioc);

  auto ep = boost::asio::connect(ws.next_layer(), results);

  host += ':' + std::to_string(ep.port());

  ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::client));

  ws.set_option(boost::beast::websocket::stream_base::decorator(
    [](boost::beast::websocket::request_type &req) {
      req.set(boost::beast::http::field::user_agent, "Copper");
    }));

  ws.handshake(host, "/");
  ws.write(boost::asio::buffer(std::string("hello")));
  boost::beast::flat_buffer buffer;
  ws.read(buffer);

  boost::beast::error_code ec;
  ws.close(boost::beast::websocket::close_code::normal, ec);
  if (ec) {
    std::cerr << "Error cerrando WebSocket: " << ec.message() << std::endl;
  } else {
    std::cout << "Conexión WebSocket cerrada correctamente." << std::endl;
  }

  std::cout << boost::beast::make_printable(buffer.data()) << std::endl;

  boost::asio::co_spawn(boost::asio::make_strand(ioc), cancel_websocket_session(ioc), boost::asio::detached);

  client_ioc.run();

  while (!thread.joinable()) {
    usleep(100);
  }
  thread.join();

  ASSERT_TRUE(true);

  sleep(10);
}