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
#include <copper/components/json.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/response.hpp>
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

    auto const _address = boost::asio::ip::make_address("0.0.0.0");
    auto const _port = 9001;
    auto const _endpoint = boost::asio::ip::tcp::endpoint{_address, _port};
    auto const _doc_root = std::string_view{"."};
    auto const _threads = 4;

    boost::asio::io_context _ioc{_threads};

    boost::asio::ssl::context _ctx{boost::asio::ssl::context::tlsv12};

    load_server_certificate(_ctx);

    auto task_group_ = boost::make_shared<task_group>(_ioc.get_executor());

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
        boost::asio::make_strand(_ioc),
        listener(_server_id, _state, task_group_, _ctx, _endpoint, _doc_root),
        task_group_->adapt([](std::exception_ptr e) {
          if (e) {
            try {
              std::rethrow_exception(e);
            } catch (std::exception &e) {
            }
          }
        }));

    boost::asio::co_spawn(boost::asio::make_strand(_ioc), subscriber(_state),
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

    boost::asio::co_spawn(boost::asio::make_strand(_ioc),
                          signal_handler(task_group_), boost::asio::detached);

    boost::asio::io_context _client_ioc;

    std::vector<std::thread> _v;
    _v.reserve(_threads);
    for (auto i = _threads; i > 0; --i)
      _v.emplace_back([&_ioc, i] { _ioc.run(); });

    sleep(1);

    boost::asio::ip::tcp::resolver _resolver(_client_ioc);
    boost::beast::tcp_stream _stream(_client_ioc);

    auto const _host = "127.0.0.1";
    auto const _results = _resolver.resolve(_host, "9001");

    _stream.connect(_results);

    {
      // Not found
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::get, "/", 11};

      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_EQ(_response.body(), "{}");
      ASSERT_EQ(_response.result_int(), 404);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {
      // Not found compressed
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::get, "/", 11};

      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");
      _request.set(fields::accept_encoding, "gzip");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_EQ(gunzip_decompress(_response.body()), "{}");
      ASSERT_EQ(_response.result_int(), 404);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::content_encoding) > 0);
      ASSERT_EQ(_response.at(fields::content_encoding), "gzip");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {
      // Not found HTML
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::get, "/", 11};

      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");
      _request.set(fields::accept, "text/html");
      _request.set(fields::accept_encoding, "gzip");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_TRUE(boost::starts_with(gunzip_decompress(_response.body()),
                                     R"(<!doctype html>)"));
      ASSERT_EQ(_response.result_int(), 404);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "text/html");

      ASSERT_TRUE(_response.count(fields::content_encoding) > 0);
      ASSERT_EQ(_response.at(fields::content_encoding), "gzip");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {
      // Options
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::options, "/api/up", 11};

      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_EQ(_response.body(), "{}");
      ASSERT_EQ(_response.result_int(), 200);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count(fields::access_control_allow_methods) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_methods), "GET");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {
      // Options compressed
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::options, "/api/up", 11};

      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");
      _request.set(fields::accept_encoding, "gzip");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_EQ(gunzip_decompress(_response.body()), "{}");
      ASSERT_EQ(_response.result_int(), 200);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::content_encoding) > 0);
      ASSERT_EQ(_response.at(fields::content_encoding), "gzip");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count(fields::access_control_allow_methods) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_methods), "GET");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {
      // Rate limiter
      for (int i = 0; i < 5; i++) {
        boost::beast::flat_buffer _buffer;
        response _response;
        request _request{method::get, "/api/up", 11};
        _request.set(fields::host, _host);
        _request.set(fields::user_agent, "Copper");

        boost::beast::http::write(_stream, _request);
        boost::beast::http::read(_stream, _buffer, _response);

        ASSERT_TRUE(boost::starts_with(_response.body(), R"({"timestamp":)"));
        ASSERT_EQ(_response.result_int(), 200);

        ASSERT_TRUE(_response.count(fields::content_type) > 0);
        ASSERT_EQ(_response.at(fields::content_type), "application/json");

        ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
        ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

        ASSERT_TRUE(_response.count("X-Server") > 0);
        ASSERT_EQ(_response.at("X-Server"), "Copper");

        ASSERT_TRUE(_response.count("X-Time") > 0);

        _buffer.clear();
        _response.clear();
      }
    }

    {
      // Too many requests
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::get, "/api/up", 11};
      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_EQ(_response.body(), "{}");
      ASSERT_EQ(_response.result_int(), 429);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);
      ASSERT_TRUE(_response.count("X-Rate-Until") > 0);

      _buffer.clear();
      _response.clear();
    }

    {
      // Too many requests compressed
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::get, "/api/up", 11};

      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");
      _request.set(fields::accept_encoding, "gzip");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_EQ(gunzip_decompress(_response.body()), "{}");
      ASSERT_EQ(_response.result_int(), 429);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::content_encoding) > 0);
      ASSERT_EQ(_response.at(fields::content_encoding), "gzip");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);
      ASSERT_TRUE(_response.count("X-Rate-Until") > 0);

      _buffer.clear();
      _response.clear();
    }

    {  // Exception
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::get, "/api/exception", 11};

      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_EQ(_response.body(), "{}");
      ASSERT_EQ(_response.result_int(), 500);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {  // Exception compressed
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::get, "/api/exception", 11};
      _request.set(fields::accept_encoding, "gzip");

      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_EQ(gunzip_decompress(_response.body()), "{}");
      ASSERT_EQ(_response.result_int(), 500);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::content_encoding) > 0);
      ASSERT_EQ(_response.at(fields::content_encoding), "gzip");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::post, "/api/auth", 11};
      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");
      _request.set(fields::content_type, "application/json");
      _request.set(fields::content_length, "53");
      boost::json::object existing_user = {{"email", "iantorres@outlook.com"},
                                           {"password", "abcdef"}};
      _request.body() = serialize(existing_user);
      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_TRUE(boost::starts_with(_response.body(), R"({"token":"Bearer)"));
      ASSERT_EQ(_response.result_int(), 200);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      auto json_response = boost::json::parse(_response.body());

      _response.clear();

      {  // User
        boost::beast::flat_buffer _user_buffer;
        response _user_response;
        request _user_request{method::get, "/api/user", 11};
        _user_request.set(fields::host, _host);
        _user_request.set(fields::user_agent, "Copper");
        _user_request.set(fields::authorization,
                          json_response.as_object().at("token").as_string());

        boost::beast::http::write(_stream, _user_request);
        boost::beast::http::read(_stream, _user_buffer, _user_response);

        ASSERT_TRUE(boost::starts_with(_user_response.body(), R"({"id":")"));
        ASSERT_TRUE(boost::contains(_user_response.body(),
                                    "75a02add-cd16-4517-9c40-b57041eb2162"));
        ASSERT_EQ(_user_response.result_int(), 200);

        ASSERT_TRUE(_user_response.count(fields::content_type) > 0);
        ASSERT_EQ(_user_response.at(fields::content_type), "application/json");

        ASSERT_TRUE(_user_response.count(fields::access_control_allow_origin) >
                    0);
        ASSERT_EQ(_user_response.at(fields::access_control_allow_origin), "*");

        ASSERT_TRUE(_user_response.count("X-Server") > 0);
        ASSERT_EQ(_user_response.at("X-Server"), "Copper");

        ASSERT_TRUE(_user_response.count("X-Time") > 0);

        _user_buffer.clear();
        _user_response.clear();
      }

      {  // User compressed
        boost::beast::flat_buffer _user_buffer;
        response _user_response;
        request _user_request{method::get, "/api/user", 11};
        _user_request.set(fields::host, _host);
        _user_request.set(fields::user_agent, "Copper");
        _user_request.set(fields::accept_encoding, "gzip");
        _user_request.set(fields::authorization,
                          json_response.as_object().at("token").as_string());

        boost::beast::http::write(_stream, _user_request);
        boost::beast::http::read(_stream, _user_buffer, _user_response);

        ASSERT_TRUE(boost::starts_with(gunzip_decompress(_user_response.body()),
                                       R"({"id":")"));
        ASSERT_TRUE(boost::contains(gunzip_decompress(_user_response.body()),
                                    "75a02add-cd16-4517-9c40-b57041eb2162"));
        ASSERT_EQ(_user_response.result_int(), 200);

        ASSERT_TRUE(_user_response.count(fields::content_type) > 0);
        ASSERT_EQ(_user_response.at(fields::content_type), "application/json");

        ASSERT_TRUE(_user_response.count(fields::content_encoding) > 0);
        ASSERT_EQ(_user_response.at(fields::content_encoding), "gzip");

        ASSERT_TRUE(_user_response.count(fields::access_control_allow_origin) >
                    0);
        ASSERT_EQ(_user_response.at(fields::access_control_allow_origin), "*");

        ASSERT_TRUE(_user_response.count("X-Server") > 0);
        ASSERT_EQ(_user_response.at("X-Server"), "Copper");

        ASSERT_TRUE(_user_response.count("X-Time") > 0);

        _user_buffer.clear();
        _user_response.clear();
      }
    }

    {
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::post, "/api/auth", 11};
      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");
      _request.set(fields::content_type, "application/json");
      _request.set(fields::content_length, "53");
      boost::json::object wrong_user = {{"email", "iantorres@outlook.com"},
                                        {"password", "defabc"}};
      _request.body() = serialize(wrong_user);
      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_TRUE(boost::starts_with(_response.body(), R"({"message":")"));
      ASSERT_TRUE(boost::contains(_response.body(),
                                  "Password provided doesn't match."));
      ASSERT_EQ(_response.result_int(), 401);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::post, "/api/auth", 11};
      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");
      _request.set(fields::content_type, "application/json");
      _request.set(fields::content_length, "47");
      boost::json::object non_registered_user = {{"email", "ian@zentrack.cl"},
                                                 {"password", "abcdef"}};
      _request.body() = serialize(non_registered_user);

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_TRUE(boost::starts_with(_response.body(), R"({"message":")"));
      ASSERT_TRUE(boost::contains(_response.body(),
                                  "Email provided isn't registered."));
      ASSERT_EQ(_response.result_int(), 401);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::get, "/api/params/hello", 11};
      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_TRUE(boost::starts_with(_response.body(), R"({"message":")"));
      ASSERT_TRUE(
          boost::contains(_response.body(), "Request has been processed."));
      ASSERT_TRUE(boost::contains(_response.body(), "hello"));
      ASSERT_EQ(_response.result_int(), 200);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::get, "/api/params/hello?a=b&c=d&e[]=f&e[]=g",
                       11};
      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_TRUE(boost::starts_with(_response.body(), R"({"message":")"));
      ASSERT_TRUE(
          boost::contains(_response.body(), "Request has been processed."));
      ASSERT_TRUE(boost::contains(_response.body(), "hello"));
      ASSERT_EQ(_response.result_int(), 200);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {  // Bad _request
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::get, "/api/../bad_request", 11};
      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_EQ(_response.body(), "{}");
      ASSERT_EQ(_response.result_int(), 400);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {  // Bad _request compressed
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::get, "/api/../bad_request", 11};

      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");
      _request.set(fields::accept_encoding, "gzip");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_EQ(gunzip_decompress(_response.body()), "{}");
      ASSERT_EQ(_response.result_int(), 400);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::content_encoding) > 0);
      ASSERT_EQ(_response.at(fields::content_encoding), "gzip");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {  // Unauthorized
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::get, "/api/user", 11};

      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_EQ(_response.body(), "{}");
      ASSERT_EQ(_response.result_int(), 401);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {  // Unauthorized compressed
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::get, "/api/user", 11};
      _request.set(fields::accept_encoding, "gzip");

      _request.set(fields::host, _host);
      _request.set(fields::user_agent, "Copper");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_EQ(gunzip_decompress(_response.body()), "{}");
      ASSERT_EQ(_response.result_int(), 401);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "application/json");

      ASSERT_TRUE(_response.count(fields::content_encoding) > 0);
      ASSERT_EQ(_response.at(fields::content_encoding), "gzip");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::get, "/", 11};
      _request.set(fields::host, _host);
      _request.set(fields::accept, "text/html");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_TRUE(boost::starts_with(_response.body(), R"(<!doctype html>)"));
      ASSERT_EQ(_response.result_int(), 404);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "text/html");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    {
      boost::beast::flat_buffer _buffer;
      response _response;
      request _request{method::get, "/api/templated", 11};
      _request.set(fields::host, _host);
      _request.set(fields::accept, "text/html");

      boost::beast::http::write(_stream, _request);
      boost::beast::http::read(_stream, _buffer, _response);

      ASSERT_TRUE(boost::starts_with(_response.body(), R"(<!doctype html>)"));
      ASSERT_EQ(_response.result_int(), 200);

      ASSERT_TRUE(_response.count(fields::content_type) > 0);
      ASSERT_EQ(_response.at(fields::content_type), "text/html");

      ASSERT_TRUE(_response.count(fields::access_control_allow_origin) > 0);
      ASSERT_EQ(_response.at(fields::access_control_allow_origin), "*");

      ASSERT_TRUE(_response.count("X-Server") > 0);
      ASSERT_EQ(_response.at("X-Server"), "Copper");

      ASSERT_TRUE(_response.count("X-Time") > 0);

      _buffer.clear();
      _response.clear();
    }

    boost::system::error_code ec;

    _stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (ec && ec != boost::system::errc::not_connected) {
    }

    _stream.close();

    _client_ioc.run();

    sleep(5);

    boost::asio::co_spawn(boost::asio::make_strand(_ioc),
                          cancel_http_sessions(), boost::asio::detached);
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