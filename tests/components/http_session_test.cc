#include <gtest/gtest.h>

#include <boost/asio/co_spawn.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/dotenv.hpp>
#include <copper/components/http_controller.hpp>
#include <copper/components/http_fields.hpp>
#include <copper/components/http_response.hpp>
#include <copper/components/json.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/server_certificates.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/signal_handler.hpp>
#include <copper/components/state.hpp>
#include <copper/components/task_group.hpp>
#include <copper/controllers/auth_controller.hpp>
#include <copper/controllers/up_controller.hpp>
#include <copper/controllers/user_controller.hpp>

using namespace copper::components;

containers::async_of<void> cancel_http_sessions() {
  auto executor = co_await boost::asio::this_coro::executor;
  executor.get_inner_executor().context().stop();
}

class exception_controller final : public http_controller {
 public:
  containers::async_of<copper::components::http_response> invoke(
      const http_request &request) override {
    throw std::runtime_error("Something went wrong");
    auto now = chronos::now();
    const json::object data = {{"message", "Request has been processed."},
                               {"data", "pong"},
                               {"timestamp", now},
                               {"status", 200}};
    co_return response(request, http_status_code::ok, serialize(data),
                       "application/json");
  }
};

class params_controller final : public http_controller {
 public:
  containers::async_of<copper::components::http_response> invoke(
      const http_request &request) override {
    auto now = chronos::now();
    const json::object data = {{"message", "Request has been processed."},
                               {"data", this->bindings_.at("name")},
                               {"timestamp", now},
                               {"status", 200}};
    co_return response(request, http_status_code::ok, serialize(data),
                       "application/json");
  }
};

TEST(Components_HTTP_Session, Implementation) {
  dotenv::init();
  try {
    auto const address = boost::asio::ip::make_address("0.0.0.0");
    auto const port = 9001;
    auto const endpoint = boost::asio::ip::tcp::endpoint{address, port};
    auto const doc_root = std::string_view{"."};
    auto const threads = 4;

    boost::asio::io_context ioc{threads};

    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};

    load_server_certificate(ctx);

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

    state_->get_http_router()
        ->push(http_method::get, "/api/up",
               boost::make_shared<copper::controllers::up_controller>(),
               {.use_throttler = true, .rpm = 5})
        ->push(http_method::get, "/api/params/{name}",
               boost::make_shared<params_controller>(),
               {.use_throttler = true, .rpm = 5})
        ->push(http_method::get, "/api/exception",
               boost::make_shared<exception_controller>(),
               {.use_throttler = true, .rpm = 5})
        ->push(http_method::get, "/api/user",
               boost::make_shared<copper::controllers::user_controller>(),
               {
                   .use_auth = true,
                   .use_throttler = true,
                   .rpm = 5,
               })
        ->push(http_method::post, "/api/auth",
               boost::make_shared<copper::controllers::auth_controller>(),
               {
                   .use_throttler = true,
                   .use_validator = true,
                   .use_protector = true,
                   .rpm = 5,
               });

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
    boost::beast::tcp_stream stream(client_ioc);

    auto const host = "127.0.0.1";
    auto const results = resolver.resolve(host, "9001");

    stream.connect(results);

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      http_request request{http_method::get, "/", 11};
      request.set(http_fields::host, host);
      request.set(http_fields::user_agent, "Copper");
      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);
      buffer.clear();
      response.clear();
    }

    {
      for (int i = 0; i <= 5; i++) {
        boost::beast::flat_buffer buffer;
        boost::beast::http::response<boost::beast::http::string_body> response;
        http_request request{http_method::get, "/api/up", 11};
        request.set(http_fields::host, host);
        request.set(http_fields::user_agent, "Copper");
        boost::beast::http::write(stream, request);
        boost::beast::http::read(stream, buffer, response);
        buffer.clear();
        std::cout << "Too Many Request: " << response << std::endl << std::endl;
        response.clear();
      }
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      http_request request{http_method::options, "/api/up", 11};
      request.set(http_fields::host, host);
      request.set(http_fields::user_agent, "Copper");
      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);
      buffer.clear();
      response.clear();
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      http_request request{http_method::get, "/api/exception", 11};
      request.set(http_fields::host, host);
      request.set(http_fields::user_agent, "Copper");
      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);
      buffer.clear();
      std::cout << "Exception Request: " << response << std::endl << std::endl;
      response.clear();
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      http_request request{http_method::post, "/api/auth", 11};
      request.set(http_fields::host, host);
      request.set(http_fields::user_agent, "Copper");
      request.set(http_fields::content_type, "application/json");
      request.set(http_fields::content_length, "53");
      boost::json::object existing_user = {{"email", "iantorres@outlook.com"},
                                           {"password", "abcdef"}};
      request.body() = serialize(existing_user);
      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);
      buffer.clear();
      std::cout << "Success auth Request: " << response << std::endl
                << std::endl;

      auto json_response = boost::json::parse(response.body());

      response.clear();

      {
        std::cout << "Key used: " << dotenv::getenv("APP_KEY") << std::endl
                  << std::endl;
        std::cout << "Token that will be used: "
                  << json_response.as_object().at("token").as_string()
                  << std::endl
                  << std::endl;

        boost::beast::flat_buffer user_buffer;
        boost::beast::http::response<boost::beast::http::string_body>
            user_response;
        http_request user_request{http_method::get, "/api/user", 11};
        user_request.set(http_fields::host, host);
        user_request.set(http_fields::user_agent, "Copper");
        user_request.set(http_fields::authorization,
                         json_response.as_object().at("token").as_string());
        boost::beast::http::write(stream, user_request);
        boost::beast::http::read(stream, user_buffer, user_response);
        user_buffer.clear();
        std::cout << "Authenticated Request: " << user_response << std::endl
                  << std::endl;
        user_response.clear();
      }
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      http_request request{http_method::post, "/api/auth", 11};
      request.set(http_fields::host, host);
      request.set(http_fields::user_agent, "Copper");
      request.set(http_fields::content_type, "application/json");
      request.set(http_fields::content_length, "53");
      boost::json::object wrong_user = {{"email", "iantorres@outlook.com"},
                                        {"password", "defabc"}};
      request.body() = serialize(wrong_user);
      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);
      std::cout << "Wrong password auth Request: " << response << std::endl
                << std::endl;
      buffer.clear();
      response.clear();
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      http_request request{http_method::post, "/api/auth", 11};
      request.set(http_fields::host, host);
      request.set(http_fields::user_agent, "Copper");
      request.set(http_fields::content_type, "application/json");
      request.set(http_fields::content_length, "47");
      boost::json::object non_registered_user = {{"email", "ian@zentrack.cl"},
                                                 {"password", "abcdef"}};
      request.body() = serialize(non_registered_user);
      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);
      buffer.clear();
      std::cout << "Wrong email auth Request: " << response << std::endl
                << std::endl;
      response.clear();
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      http_request request{http_method::get, "/api/params/hello", 11};
      request.set(http_fields::host, host);
      request.set(http_fields::user_agent, "Copper");
      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);
      std::cout << "Params Request: " << response << std::endl << std::endl;
      buffer.clear();
      response.clear();
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      http_request request{http_method::get,
                           "/api/params/hello?a=b&c=d&e[]=f&e[]=g", 11};
      request.set(http_fields::host, host);
      request.set(http_fields::user_agent, "Copper");
      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);
      std::cout << "Query Request: " << response << std::endl << std::endl;
      buffer.clear();
      response.clear();
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      http_request request{http_method::get, "/api/../bad_request", 11};
      request.set(http_fields::host, host);
      request.set(http_fields::user_agent, "Copper");
      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);
      buffer.clear();
      std::cout << "Bad Request: " << response << std::endl << std::endl;
      response.clear();
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      http_request request{http_method::get, "/api/user", 11};
      request.set(http_fields::host, host);
      request.set(http_fields::user_agent, "Copper");
      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);
      buffer.clear();
      std::cout << "User Request: " << response << std::endl << std::endl;
      response.clear();
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      http_request request{http_method::get, "/", 11};
      request.set(http_fields::host, host);
      request.set(http_fields::user_agent, "Copper");
      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);
      buffer.clear();
      std::cout << "Close Request: " << response << std::endl << std::endl;
      response.clear();
    }

    boost::system::error_code ec;

    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (ec && ec != boost::system::errc::not_connected) {
      std::cerr << "Error en shutdown: " << ec.message() << std::endl;
    }

    stream.close();

    client_ioc.run();

    sleep(5);

    boost::asio::co_spawn(boost::asio::make_strand(ioc), cancel_http_sessions(),
                          boost::asio::detached);
    ioc.stop();

    sleep(5);

    for (auto &t : v) t.join();

    ASSERT_TRUE(true);

  } catch (std::runtime_error &e) {
  } catch (std::exception &e) {
  } catch (...) {
  }
}