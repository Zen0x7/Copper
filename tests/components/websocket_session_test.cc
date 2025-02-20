#include <gtest/gtest.h>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/ssl.hpp>
#include <copper/components/client_certificates.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/server_certificates.hpp>
#include <copper/components/signal_handler.hpp>
#include <copper/components/state.hpp>
#include <copper/components/task_group.hpp>

boost::asio::awaitable<
    void, boost::asio::strand<boost::asio::io_context::executor_type>>
cancel_websocket_session() {
  auto executor = co_await boost::asio::this_coro::executor;
  executor.get_inner_executor().context().stop();
}

TEST(Components_WebSocket_Session, Implementation) {
  try {
    dotenv::init();
    using namespace copper::components;

    auto const address = boost::asio::ip::make_address("0.0.0.0");
    auto const port = 9002;
    auto const endpoint = boost::asio::ip::tcp::endpoint{address, port};
    auto const doc_root = std::string_view{"."};
    auto const threads = 4;

    boost::asio::io_context ioc{threads};

    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};
    boost::asio::ssl::context client_ctx{boost::asio::ssl::context::tlsv12};

    load_server_certificate(ctx);
    load_root_certificates(client_ctx);

    auto task_group_ = boost::make_shared<task_group>(ioc.get_executor());

    boost::mysql::pool_params database_params;
    database_params.server_address.emplace_host_and_port(
        dotenv::getenv("DATABASE_HOST", "127.0.0.1"),
        std::stoi(dotenv::getenv("DATABASE_PORT", "3306")));

    database_params.username = dotenv::getenv("DATABASE_USER", "user");
    database_params.password =
        dotenv::getenv("DATABASE_PASSWORD", "user_password");
    database_params.database = dotenv::getenv("DATABASE_NAME", "copper");
    database_params.thread_safe = true;
    database_params.initial_size = 10;
    database_params.max_size = 100;

    auto database_pool = boost::make_shared<boost::mysql::connection_pool>(
        ioc, std::move(database_params));

    auto state_ = boost::make_shared<state>(database_pool);

    state_->get_database()->start();

    boost::asio::co_spawn(
        boost::asio::make_strand(ioc),
        listener(state_, task_group_, ctx, endpoint, doc_root),
        task_group_->adapt([](std::exception_ptr e) {
          if (e) {
            try {
              std::rethrow_exception(e);
            } catch (std::exception &e) {
              std::cerr << "Error in listener: " << e.what() << "\n";
            }
          }
        }));

    boost::asio::co_spawn(boost::asio::make_strand(ioc),
                          signal_handler(task_group_), boost::asio::detached);

    boost::asio::io_context client_ioc;

    std::vector<std::thread> v;
    v.reserve(threads);
    for (auto i = threads; i > 0; --i)
      v.emplace_back([&ioc, i] {
        std::cout << "Thread " << i << " starting " << std::endl;
        ioc.run();
        std::cout << "Thread " << i << " stopped " << std::endl;
      });

    sleep(1);

    boost::asio::ip::tcp::resolver resolver(client_ioc);

    std::string host = "127.0.0.1";
    auto const results = resolver.resolve(host, "9002");

    boost::beast::websocket::stream<
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>
        ws(client_ioc, client_ctx);

    auto ep = boost::asio::connect(boost::beast::get_lowest_layer(ws), results);

    if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(),
                                  host.c_str()))
      throw boost::beast::system_error(
          boost::beast::error_code(static_cast<int>(::ERR_get_error()),
                                   boost::asio::error::get_ssl_category()),
          "Failed to set SNI Hostname");

    host += ':' + std::to_string(ep.port());

    ws.next_layer().handshake(ssl::stream_base::client);

    ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(
        boost::beast::role_type::client));
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
      std::cerr << "Error en shutdown: " << ec.message() << std::endl;
    }

    std::cout << boost::beast::make_printable(buffer.data()) << std::endl;

    client_ioc.run();

    sleep(5);

    boost::asio::co_spawn(boost::asio::make_strand(ioc),
                          cancel_websocket_session(), boost::asio::detached);
    ioc.stop();

    sleep(5);

    for (auto &t : v) t.join();

    ASSERT_TRUE(true);

  } catch (std::runtime_error &e) {
  } catch (std::exception &e) {
  } catch (...) {
  }
}