#include <gtest/gtest.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/gunzip.hpp>
#include <copper/components/response.hpp>
#include <copper/components/json.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/router.hpp>
#include <copper/components/server_certificates.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/signal_handler.hpp>
#include <copper/components/state.hpp>
#include <copper/components/subscriber.hpp>
#include <copper/components/task_group.hpp>
#include <copper/controllers/api/auth_controller.hpp>
#include <copper/controllers/api/up_controller.hpp>
#include <copper/controllers/api/user_controller.hpp>
#include <nlohmann/json.hpp>

using namespace copper::components;

containers::async_of<void> cancel_http_sessions() {
  auto executor = co_await boost::asio::this_coro::executor;
  executor.get_inner_executor().context().stop();
}

class templated_controller final : public controller {
 public:
  containers::async_of<copper::components::response> invoke(
      const request &request) override {
    json::json data;
    co_return make_view(request, status_code::ok, "template", data);
  }
};

class exception_controller final : public controller {
 public:
  containers::async_of<copper::components::response> invoke(
      const request &request) override {
    throw std::runtime_error("Something went wrong");
    auto now = chronos::now();
    const json::object data = {{"message", "Request has been processed."},
                               {"data", "pong"},
                               {"timestamp", now},
                               {"status", 200}};
    co_return make_response(request, status_code::ok, serialize(data),
                            "application/json");
  }
};

class params_controller final : public controller {
 public:
  containers::async_of<copper::components::response> invoke(
      const request &request) override {
    auto now = chronos::now();
    const json::object data = {{"message", "Request has been processed."},
                               {"data", this->bindings_.at("name")},
                               {"timestamp", now},
                               {"status", 200}};
    co_return make_response(request, status_code::ok, serialize(data),
                            "application/json");
  }
};

TEST(Components_HTTP_Session, Implementation) {
  try {
    auto _server_id = boost::uuids::random_generator()();
    auto _configuration = boost::make_shared<configuration>();

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
        _configuration->get()->database_host_,
        _configuration->get()->database_port_);

    database_params.username = _configuration->get()->database_user_;
    database_params.password = _configuration->get()->database_password_;
    database_params.database = _configuration->get()->database_name_;
    database_params.thread_safe =
        _configuration->get()->database_pool_thread_safe_;
    database_params.initial_size =
        _configuration->get()->database_pool_initial_size_;
    database_params.max_size = _configuration->get()->database_pool_max_size_;

    auto database_pool = boost::make_shared<boost::mysql::connection_pool>(
        ioc, std::move(database_params));

    auto _state = boost::make_shared<state>(_configuration, database_pool);

    _state->get_database()->start();

    _state->get_router()
        ->push(method::get, "/api/up",
               boost::make_shared<copper::controllers::api::up_controller>(),
               {.use_throttler_ = true, .use_protector_ = false, .rpm_ = 5})
        ->push(method::get, "/api/templated",
               boost::make_shared<templated_controller>(),
               {.use_throttler_ = false, .use_protector_ = false})
        ->push(method::get, "/api/params/{name}",
               boost::make_shared<params_controller>(),
               {.use_throttler_ = true, .use_protector_ = false, .rpm_ = 5})
        ->push(method::get, "/api/exception",
               boost::make_shared<exception_controller>(),
               {.use_throttler_ = true, .use_protector_ = false, .rpm_ = 5})
        ->push(method::get, "/api/user",
               boost::make_shared<copper::controllers::api::user_controller>(),
               {
                   .use_auth_ = true,
                   .use_throttler_ = true,
                   .use_protector_ = false,
                   .rpm_ = 5,
               })
        ->push(method::post, "/api/auth",
               boost::make_shared<copper::controllers::api::auth_controller>(),
               {
                   .use_throttler_ = true,
                   .use_validator_ = true,
                   .use_protector_ = true,
                   .rpm_ = 5,
               });

    boost::asio::co_spawn(
        boost::asio::make_strand(ioc),
        listener(_server_id, _state, task_group_, ctx, endpoint, doc_root),
        task_group_->adapt([](std::exception_ptr e) {
          if (e) {
            try {
              std::rethrow_exception(e);
            } catch (std::exception &e) {
            }
          }
        }));

    boost::asio::co_spawn(boost::asio::make_strand(ioc), subscriber(_state),
                          task_group_->adapt([](std::exception_ptr e) {
                            if (e) {
                              try {
                                std::rethrow_exception(e);
                              } catch (std::exception &e) {
                                //              std::cerr << "Error in listener:
                                //              " << e.what() <<
                                //              "\n";
                              }
                            }
                          }));

    boost::asio::co_spawn(boost::asio::make_strand(ioc),
                          signal_handler(task_group_), boost::asio::detached);

    boost::asio::io_context client_ioc;

    std::vector<std::thread> v;
    v.reserve(threads);
    for (auto i = threads; i > 0; --i) v.emplace_back([&ioc, i] { ioc.run(); });

    sleep(1);

    boost::asio::ip::tcp::resolver resolver(client_ioc);
    boost::beast::tcp_stream stream(client_ioc);

    auto const host = "127.0.0.1";
    auto const results = resolver.resolve(host, "9001");

    stream.connect(results);

    {
      // Not found
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::get, "/", 11};

      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_EQ(response.body(), "{}");
      ASSERT_EQ(response.result_int(), 404);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {
      // Not found compressed
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::get, "/", 11};

      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");
      request.set(fields::accept_encoding, "gzip");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_EQ(gunzip_decompress(response.body()), "{}");
      ASSERT_EQ(response.result_int(), 404);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::content_encoding) > 0);
      ASSERT_EQ(response.at(fields::content_encoding), "gzip");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {
      // Not found HTML
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::get, "/", 11};

      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");
      request.set(fields::accept, "text/html");
      request.set(fields::accept_encoding, "gzip");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_TRUE(boost::starts_with(gunzip_decompress(response.body()),
                                     R"(<!doctype html>)"));
      ASSERT_EQ(response.result_int(), 404);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "text/html");

      ASSERT_TRUE(response.count(fields::content_encoding) > 0);
      ASSERT_EQ(response.at(fields::content_encoding), "gzip");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {
      // Options
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::options, "/api/up", 11};

      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_EQ(response.body(), "{}");
      ASSERT_EQ(response.result_int(), 200);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count(fields::access_control_allow_methods) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_methods), "GET");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {
      // Options compressed
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::options, "/api/up", 11};

      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");
      request.set(fields::accept_encoding, "gzip");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_EQ(gunzip_decompress(response.body()), "{}");
      ASSERT_EQ(response.result_int(), 200);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::content_encoding) > 0);
      ASSERT_EQ(response.at(fields::content_encoding), "gzip");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count(fields::access_control_allow_methods) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_methods), "GET");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {
      // Rate limiter
      for (int i = 0; i < 5; i++) {
        boost::beast::flat_buffer buffer;
        boost::beast::http::response<boost::beast::http::string_body> response;
        request request{method::get, "/api/up", 11};
        request.set(fields::host, host);
        request.set(fields::user_agent, "Copper");

        boost::beast::http::write(stream, request);
        boost::beast::http::read(stream, buffer, response);

        ASSERT_TRUE(boost::starts_with(response.body(), R"({"timestamp":)"));
        ASSERT_EQ(response.result_int(), 200);

        ASSERT_TRUE(response.count(fields::content_type) > 0);
        ASSERT_EQ(response.at(fields::content_type), "application/json");

        ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
        ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

        ASSERT_TRUE(response.count("X-Server") > 0);
        ASSERT_EQ(response.at("X-Server"), "Copper");

        ASSERT_TRUE(response.count("X-Time") > 0);

        buffer.clear();
        response.clear();
      }
    }

    {
      // Too many requests
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::get, "/api/up", 11};
      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_EQ(response.body(), "{}");
      ASSERT_EQ(response.result_int(), 429);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);
      ASSERT_TRUE(response.count("X-Rate-Until") > 0);

      buffer.clear();
      response.clear();
    }

    {
      // Too many requests compressed
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::get, "/api/up", 11};

      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");
      request.set(fields::accept_encoding, "gzip");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_EQ(gunzip_decompress(response.body()), "{}");
      ASSERT_EQ(response.result_int(), 429);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::content_encoding) > 0);
      ASSERT_EQ(response.at(fields::content_encoding), "gzip");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);
      ASSERT_TRUE(response.count("X-Rate-Until") > 0);

      buffer.clear();
      response.clear();
    }

    {  // Exception
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::get, "/api/exception", 11};

      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_EQ(response.body(), "{}");
      ASSERT_EQ(response.result_int(), 500);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {  // Exception compressed
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::get, "/api/exception", 11};
      request.set(fields::accept_encoding, "gzip");

      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_EQ(gunzip_decompress(response.body()), "{}");
      ASSERT_EQ(response.result_int(), 500);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::content_encoding) > 0);
      ASSERT_EQ(response.at(fields::content_encoding), "gzip");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::post, "/api/auth", 11};
      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");
      request.set(fields::content_type, "application/json");
      request.set(fields::content_length, "53");
      boost::json::object existing_user = {{"email", "iantorres@outlook.com"},
                                           {"password", "abcdef"}};
      request.body() = serialize(existing_user);
      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_TRUE(boost::starts_with(response.body(), R"({"token":"Bearer)"));
      ASSERT_EQ(response.result_int(), 200);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      auto json_response = boost::json::parse(response.body());

      response.clear();

      {  // User
        boost::beast::flat_buffer user_buffer;
        boost::beast::http::response<boost::beast::http::string_body>
            user_response;
        request user_request{method::get, "/api/user", 11};
        user_request.set(fields::host, host);
        user_request.set(fields::user_agent, "Copper");
        user_request.set(fields::authorization,
                         json_response.as_object().at("token").as_string());

        boost::beast::http::write(stream, user_request);
        boost::beast::http::read(stream, user_buffer, user_response);

        ASSERT_TRUE(boost::starts_with(user_response.body(), R"({"id":")"));
        ASSERT_TRUE(boost::contains(user_response.body(),
                                    "75a02add-cd16-4517-9c40-b57041eb2162"));
        ASSERT_EQ(user_response.result_int(), 200);

        ASSERT_TRUE(user_response.count(fields::content_type) > 0);
        ASSERT_EQ(user_response.at(fields::content_type), "application/json");

        ASSERT_TRUE(user_response.count(fields::access_control_allow_origin) >
                    0);
        ASSERT_EQ(user_response.at(fields::access_control_allow_origin), "*");

        ASSERT_TRUE(user_response.count("X-Server") > 0);
        ASSERT_EQ(user_response.at("X-Server"), "Copper");

        ASSERT_TRUE(user_response.count("X-Time") > 0);

        user_buffer.clear();
        user_response.clear();
      }

      {  // User compressed
        boost::beast::flat_buffer user_buffer;
        boost::beast::http::response<boost::beast::http::string_body>
            user_response;
        request user_request{method::get, "/api/user", 11};
        user_request.set(fields::host, host);
        user_request.set(fields::user_agent, "Copper");
        user_request.set(fields::accept_encoding, "gzip");
        user_request.set(fields::authorization,
                         json_response.as_object().at("token").as_string());

        boost::beast::http::write(stream, user_request);
        boost::beast::http::read(stream, user_buffer, user_response);

        ASSERT_TRUE(boost::starts_with(gunzip_decompress(user_response.body()),
                                       R"({"id":")"));
        ASSERT_TRUE(boost::contains(gunzip_decompress(user_response.body()),
                                    "75a02add-cd16-4517-9c40-b57041eb2162"));
        ASSERT_EQ(user_response.result_int(), 200);

        ASSERT_TRUE(user_response.count(fields::content_type) > 0);
        ASSERT_EQ(user_response.at(fields::content_type), "application/json");

        ASSERT_TRUE(user_response.count(fields::content_encoding) > 0);
        ASSERT_EQ(user_response.at(fields::content_encoding), "gzip");

        ASSERT_TRUE(user_response.count(fields::access_control_allow_origin) >
                    0);
        ASSERT_EQ(user_response.at(fields::access_control_allow_origin), "*");

        ASSERT_TRUE(user_response.count("X-Server") > 0);
        ASSERT_EQ(user_response.at("X-Server"), "Copper");

        ASSERT_TRUE(user_response.count("X-Time") > 0);

        user_buffer.clear();
        user_response.clear();
      }
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::post, "/api/auth", 11};
      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");
      request.set(fields::content_type, "application/json");
      request.set(fields::content_length, "53");
      boost::json::object wrong_user = {{"email", "iantorres@outlook.com"},
                                        {"password", "defabc"}};
      request.body() = serialize(wrong_user);
      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_TRUE(boost::starts_with(response.body(), R"({"message":")"));
      ASSERT_TRUE(
          boost::contains(response.body(), "Password provided doesn't match."));
      ASSERT_EQ(response.result_int(), 401);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::post, "/api/auth", 11};
      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");
      request.set(fields::content_type, "application/json");
      request.set(fields::content_length, "47");
      boost::json::object non_registered_user = {{"email", "ian@zentrack.cl"},
                                                 {"password", "abcdef"}};
      request.body() = serialize(non_registered_user);

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_TRUE(boost::starts_with(response.body(), R"({"message":")"));
      ASSERT_TRUE(
          boost::contains(response.body(), "Email provided isn't registered."));
      ASSERT_EQ(response.result_int(), 401);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::get, "/api/params/hello", 11};
      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_TRUE(boost::starts_with(response.body(), R"({"message":")"));
      ASSERT_TRUE(
          boost::contains(response.body(), "Request has been processed."));
      ASSERT_TRUE(boost::contains(response.body(), "hello"));
      ASSERT_EQ(response.result_int(), 200);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::get, "/api/params/hello?a=b&c=d&e[]=f&e[]=g",
                      11};
      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_TRUE(boost::starts_with(response.body(), R"({"message":")"));
      ASSERT_TRUE(
          boost::contains(response.body(), "Request has been processed."));
      ASSERT_TRUE(boost::contains(response.body(), "hello"));
      ASSERT_EQ(response.result_int(), 200);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {  // Bad request
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::get, "/api/../bad_request", 11};
      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_EQ(response.body(), "{}");
      ASSERT_EQ(response.result_int(), 400);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {  // Bad request compressed
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::get, "/api/../bad_request", 11};

      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");
      request.set(fields::accept_encoding, "gzip");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_EQ(gunzip_decompress(response.body()), "{}");
      ASSERT_EQ(response.result_int(), 400);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::content_encoding) > 0);
      ASSERT_EQ(response.at(fields::content_encoding), "gzip");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {  // Unauthorized
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::get, "/api/user", 11};

      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_EQ(response.body(), "{}");
      ASSERT_EQ(response.result_int(), 401);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {  // Unauthorized compressed
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::get, "/api/user", 11};
      request.set(fields::accept_encoding, "gzip");

      request.set(fields::host, host);
      request.set(fields::user_agent, "Copper");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_EQ(gunzip_decompress(response.body()), "{}");
      ASSERT_EQ(response.result_int(), 401);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "application/json");

      ASSERT_TRUE(response.count(fields::content_encoding) > 0);
      ASSERT_EQ(response.at(fields::content_encoding), "gzip");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::get, "/", 11};
      request.set(fields::host, host);
      request.set(fields::accept, "text/html");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_TRUE(boost::starts_with(response.body(), R"(<!doctype html>)"));
      ASSERT_EQ(response.result_int(), 404);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "text/html");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    {
      boost::beast::flat_buffer buffer;
      boost::beast::http::response<boost::beast::http::string_body> response;
      request request{method::get, "/api/templated", 11};
      request.set(fields::host, host);
      request.set(fields::accept, "text/html");

      boost::beast::http::write(stream, request);
      boost::beast::http::read(stream, buffer, response);

      ASSERT_TRUE(boost::starts_with(response.body(), R"(<!doctype html>)"));
      ASSERT_EQ(response.result_int(), 200);

      ASSERT_TRUE(response.count(fields::content_type) > 0);
      ASSERT_EQ(response.at(fields::content_type), "text/html");

      ASSERT_TRUE(response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(response.count("X-Server") > 0);
      ASSERT_EQ(response.at("X-Server"), "Copper");

      ASSERT_TRUE(response.count("X-Time") > 0);

      buffer.clear();
      response.clear();
    }

    boost::system::error_code ec;

    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (ec && ec != boost::system::errc::not_connected) {
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
    std::cout << e.what() << std::endl;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  } catch (...) {
    std::cout << "Something went wrong" << std::endl;
  }
}