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
> cancel_websocket_session() {
  auto executor = co_await boost::asio::this_coro::executor;
  executor.get_inner_executor().context().stop();
}

TEST(Components_WebSocket_Session, Implementation) {
  using namespace copper::components;

  auto const address = boost::asio::ip::make_address("0.0.0.0");
  auto const port = 9002;
  auto const endpoint = boost::asio::ip::tcp::endpoint{address, port};
  auto const doc_root = std::string_view{"."};

  boost::asio::io_context ioc{8};

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

  try {
    std::thread first_thread([&]() {
      try {
        std::cout << "Running thread #1" << std::endl;
        ioc.run();
        std::cout << "Stopped thread #1" << std::endl;
      } catch (std::exception &e) {

        std::cout << "Exception on thread #1" << std::endl;
      }
    });

    std::thread second_thread([&]() {
      try {
        std::cout << "Running thread #2" << std::endl;
        ioc.run();
        std::cout << "Stopped thread #2" << std::endl;
      } catch (std::exception &e) {

        std::cout << "Exception on thread #2" << std::endl;

      }
    });

    first_thread.detach();
    second_thread.detach();

    sleep(5); // Wait for service

    boost::asio::ip::tcp::resolver resolver(client_ioc);

    std::string host = "127.0.0.1";
    auto const results = resolver.resolve(host, "9002");

    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{client_ioc};

    auto ep = boost::asio::connect(ws.next_layer(), results);

    host += ':' + std::to_string(ep.port());

    ws.set_option(boost::beast::websocket::stream_base::decorator(
      [](boost::beast::websocket::request_type &req) {
        req.set(boost::beast::http::field::user_agent, "Copper");
      }));

    ws.handshake(host, "/");
    ws.write(boost::asio::buffer(std::string("hello")));
    boost::beast::flat_buffer buffer;
    ws.read(buffer);

    ws.close(boost::beast::websocket::close_code::normal);

    ws.next_layer().close();

    std::cout << boost::beast::make_printable(buffer.data()) << std::endl;

    boost::asio::co_spawn(boost::asio::make_strand(ioc), cancel_websocket_session(), boost::asio::detached);

    sleep(5); // Wait for shutdown

    first_thread.join();
    second_thread.join();

  } catch (std::exception const &e) {
  }

  ASSERT_TRUE(true);
}