#include <gtest/gtest.h>

#include <copper/components/certificates.hpp>
#include <copper/components/task_group.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/state.hpp>
#include <copper/components/signal_handler.hpp>
#include <copper/components/http_method.hpp>
#include <copper/components/http_response.hpp>
#include <copper/components/http_fields.hpp>
#include <copper/components/http_controller.hpp>
#include <copper/components/http_status_code.hpp>
#include <copper/components/json.hpp>
#include <copper/components/dotenv.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/shared.hpp>

#include <app/controllers/auth_controller.hpp>
#include <app/controllers/up_controller.hpp>
#include <app/controllers/user_controller.hpp>

#include <boost/beast.hpp>

using namespace copper::components;

boost::asio::awaitable<
  void,
  boost::asio::strand<
    boost::asio::io_context::executor_type
  >
> cancel() {
  auto executor = co_await boost::asio::this_coro::executor;
  executor.get_inner_executor().context().stop();
}

class exception_controller final : public http_controller {
public:
  bool requires_limitation() const override { return false; }

  int requests_per_minute() const override { return 5; }

  http_response invoke(const http_request &request) override {
    throw std::runtime_error("Something went wrong");
    auto now = chronos::now();
    const json::object data = {
      {"message",   "Request has been processed."},
      {"data",      "pong"},
      {"timestamp", now},
      {"status",    200}
    };
    return response(request, http_status_code::ok, serialize(data), "application/json", now);
  }
};

class params_controller final : public http_controller {
public:
  bool requires_limitation() const override { return true; }

  int requests_per_minute() const override { return 5; }

  http_response invoke(const http_request &request) override {
    auto now = chronos::now();
    const json::object data = {
      {"message",   "Request has been processed."},
      {"data",      this->bindings_.at("name")},
      {"timestamp", now},
      {"status",    200}
    };
    return response(request, http_status_code::ok, serialize(data), "application/json", now);
  }
};

TEST(Components_HTTP_Session, Implementation) {
  dotenv::init();

  auto const address = boost::asio::ip::make_address("0.0.0.0");
  auto const port = 9001;
  auto const endpoint = boost::asio::ip::tcp::endpoint{address, port};
  auto const doc_root = std::string_view{"."};

  boost::asio::io_context ioc{8};

  boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};

  load_server_certificate(ctx);

  auto task_group_ = boost::make_shared<task_group>(ioc.get_executor());

  auto state_ = boost::make_shared<state>();

  state_->get_database()->start();

  state_->get_router()->get_routes()->push_back(
    std::pair(http_router::factory(http_method::get, "/api/up"), boost::make_shared<app::controllers::up_controller>())
  );

  state_->get_router()->get_routes()->push_back(
    std::pair(http_router::factory(http_method::get, "/api/params/{name}"), boost::make_shared<params_controller>())
  );

  state_->get_router()->get_routes()->push_back(
    std::pair(http_router::factory(http_method::get, "/api/exception"), boost::make_shared<exception_controller>())
  );

  state_->get_router()->get_routes()->push_back(
    std::pair(http_router::factory(http_method::get, "/api/user"), boost::make_shared<app::controllers::user_controller>())
  );

  state_->get_router()->get_routes()->push_back(
    std::pair(http_router::factory(http_method::post, "/api/auth"),
              boost::make_shared<app::controllers::auth_controller>())
  );

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

    sleep(3);

    boost::asio::ip::tcp::resolver resolver(client_ioc);
    boost::beast::tcp_stream stream(client_ioc);

    auto const host = "127.0.0.1";
    auto const results = resolver.resolve(host, "9001");

    stream.connect(results);

    http_request close_req{http_method::get, "/", 11};
    close_req.set(http_fields::host, host);
    close_req.set(http_fields::user_agent, "Copper");

    boost::beast::flat_buffer buffer;
    boost::beast::http::response<boost::beast::http::string_body> response;

    http_request req{http_method::get, "/", 11};
    req.set(http_fields::host, host);
    req.set(http_fields::user_agent, "Copper");
    boost::beast::http::response<boost::beast::http::string_body> res;
    boost::beast::http::write(stream, req);
    boost::beast::http::read(stream, buffer, res);
    buffer.clear();
    res.clear();


    http_request options_up_request{http_method::options, "/api/up", 11};
    options_up_request.set(http_fields::host, host);
    options_up_request.set(http_fields::user_agent, "Copper");
    boost::beast::http::write(stream, options_up_request);
    boost::beast::http::read(stream, buffer, response);
    buffer.clear();
    response.clear();

    http_request up_request{http_method::get, "/api/up", 11};
    up_request.set(http_fields::host, host);
    up_request.set(http_fields::user_agent, "Copper");
    for (int i = 0; i <= 5; i++) {
      boost::beast::http::write(stream, up_request);
      boost::beast::http::read(stream, buffer, response);
      std::cout << "Too Many Request: " << response << std::endl << std::endl;
      buffer.clear();
      response.clear();
    }

    http_request exception_request{http_method::get, "/api/exception", 11};
    exception_request.set(http_fields::host, host);
    exception_request.set(http_fields::user_agent, "Copper");
    boost::beast::http::write(stream, exception_request);
    boost::beast::http::read(stream, buffer, response);
    std::cout << "Exception Request: " << response << std::endl << std::endl;
    buffer.clear();
    response.clear();

    http_request success_auth_request{http_method::post, "/api/auth", 11};
    success_auth_request.set(http_fields::host, host);
    success_auth_request.set(http_fields::user_agent, "Copper");
    success_auth_request.set(http_fields::content_type, "application/json");
    success_auth_request.set(http_fields::content_length, "53");
    boost::json::object existing_user = {
      {"email",    "iantorres@outlook.com"},
      {"password", "abcdef"}
    };
    success_auth_request.body() = serialize(existing_user);
    boost::beast::http::write(stream, success_auth_request);
    boost::beast::http::read(stream, buffer, response);
    std::cout << "Success auth Request: " << response << std::endl << std::endl;
    buffer.clear();
    response.clear();

    http_request wrong_password_auth_request{http_method::post, "/api/auth", 11};
    wrong_password_auth_request.set(http_fields::host, host);
    wrong_password_auth_request.set(http_fields::user_agent, "Copper");
    wrong_password_auth_request.set(http_fields::content_type, "application/json");
    wrong_password_auth_request.set(http_fields::content_length, "53");
    boost::json::object wrong_user = {
      {"email",    "iantorres@outlook.com"},
      {"password", "defabc"}
    };
    wrong_password_auth_request.body() = serialize(wrong_user);
    boost::beast::http::write(stream, wrong_password_auth_request);
    boost::beast::http::read(stream, buffer, response);
    std::cout << "Wrong password auth Request: " << response << std::endl << std::endl;
    buffer.clear();
    response.clear();

    http_request wrong_email_auth_request{http_method::post, "/api/auth", 11};
    wrong_email_auth_request.set(http_fields::host, host);
    wrong_email_auth_request.set(http_fields::user_agent, "Copper");
    wrong_email_auth_request.set(http_fields::content_type, "application/json");
    wrong_email_auth_request.set(http_fields::content_length, "53");
    boost::json::object non_registered_user = {
      {"email",    "ian@zentrack.cl"},
      {"password", "abcdef"}
    };
    wrong_email_auth_request.body() = serialize(non_registered_user);
    boost::beast::http::write(stream, wrong_email_auth_request);
    boost::beast::http::read(stream, buffer, response);
    std::cout << "Wrong email auth Request: " << response << std::endl << std::endl;
    buffer.clear();
    response.clear();

    http_request params_request{http_method::get, "/api/params/{name}", 11};
    params_request.set(http_fields::host, host);
    params_request.set(http_fields::user_agent, "Copper");
    boost::beast::http::write(stream, params_request);
    boost::beast::http::read(stream, buffer, response);
    std::cout << "Params Request: " << response << std::endl << std::endl;
    buffer.clear();
    response.clear();

    http_request bad_request{http_method::get, "/api/../bad_request", 11};
    bad_request.set(http_fields::host, host);
    bad_request.set(http_fields::user_agent, "Copper");
    boost::beast::http::write(stream, bad_request);
    boost::beast::http::read(stream, buffer, response);
    std::cout << "Bad Request: " << response << std::endl << std::endl;
    buffer.clear();
    response.clear();

    http_request authenticated_request{http_method::get, "/api/user", 11};
    authenticated_request.set(http_fields::host, host);
    authenticated_request.set(http_fields::user_agent, "Copper");
    boost::beast::http::write(stream, authenticated_request);
    boost::beast::http::read(stream, buffer, response);
    buffer.clear();
    std::cout << "Authenticated Request: " << response << std::endl << std::endl;
    response.clear();

    boost::beast::http::response<boost::beast::http::string_body> close_res;
    boost::beast::http::write(stream, close_req);
    boost::beast::http::read(stream, buffer, close_res);
    buffer.clear();
    std::cout << "Close Request: " << close_res << std::endl << std::endl;
    close_res.clear();

    boost::system::error_code ec;

    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (ec && ec != boost::system::errc::not_connected) {
      std::cerr << "Error en shutdown: " << ec.message() << std::endl;
    }

    stream.close();

    boost::asio::co_spawn(boost::asio::make_strand(ioc), cancel(), boost::asio::detached);

    first_thread.join();
    second_thread.join();

  } catch (std::exception const &e) {

  }

  ASSERT_TRUE(true);
}