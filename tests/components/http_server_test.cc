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
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>
#include <boost/uuid/random_generator.hpp>
#include <copper/components/binaries.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/controllers/api/auth_controller.hpp>
#include <copper/components/controllers/api/register_controller.hpp>
#include <copper/components/controllers/api/up_controller.hpp>
#include <copper/components/controllers/api/user_controller.hpp>
#include <copper/components/core.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/gunzip.hpp>
#include <copper/components/http_listener.hpp>
#include <copper/components/json.hpp>
#include <copper/components/random.hpp>
#include <copper/components/response.hpp>
#include <copper/components/router.hpp>
#include <copper/components/routes.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/state.hpp>
#include <copper/components/subscriber.hpp>
#include <future>
#include <nlohmann/json.hpp>
#include <thread>

using namespace copper::components;
class templated_controller final : public controller {
 public:
  void invoke(const shared<core>& core_,
              const shared<controller_parameters> parameters_,
              const callback_of<res> on_success,
              const callback_of<std::exception_ptr>) override {
    const json::json data_;
    on_success(make_view(core_, parameters_, status_code::ok, "template", data_,
                         "text/html"));
  }
};

class exception_controller final : public controller {
 public:
  void invoke(const shared<core>&,
              const shared<controller_parameters>,
              const callback_of<res>,
              const callback_of<std::exception_ptr> on_error) override {
    on_error(
        std::make_exception_ptr(std::runtime_error("Something went wrong")));
  }
};

class params_controller final : public controller {
 public:
  void invoke(const shared<core>& core_,
              const shared<controller_parameters> parameters_,
              const callback_of<res> on_success,
              const callback_of<std::exception_ptr>) override {
    const auto now_ = chronos::now();
    const json::object response_data_ = {
        {"message", "Request has been processed."},
        {"data", parameters_->get_bindings().at("name")},
        {"timestamp", now_},
        {"status", 200}};
    on_success(make_response(core_, parameters_, status_code::ok,
                             serialize(response_data_), "application/json"));
  }
};

class HttpServerTestFixture : public ::testing::Test {
 protected:
  void SetUp() override {
    using namespace copper::components;

    std::puts("Generating server ID ...");
    server_id_ = boost::uuids::random_generator()();

    std::puts("Creating address and HTTP endpoint ...");
    address_ = boost::asio::ip::make_address("0.0.0.0");
    http_endpoint_ = boost::asio::ip::tcp::endpoint{address_, 9009};

    ioc_ = std::make_unique<boost::asio::io_context>(1);
    core_ = core::factory(*ioc_);
    core_->views_->push("404", "404");

    std::puts("Registering HTTP routes ...");
    core_->router_
        ->push(method::post, "/api/register",
               boost::make_shared<controllers::api::register_controller>(),
               {.use_throttler_ = true, .use_validator_ = true, .rpm_ = 5})
        ->push(method::get, "/api/up",
               boost::make_shared<controllers::api::up_controller>(),
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
               boost::make_shared<controllers::api::user_controller>(),
               {.use_auth_ = true,
                .use_throttler_ = true,
                .use_protector_ = false,
                .rpm_ = 5})
        ->push(method::post, "/api/auth",
               boost::make_shared<controllers::api::auth_controller>(),
               {.use_throttler_ = true,
                .use_validator_ = true,
                .use_protector_ = true,
                .rpm_ = 5});

    std::puts("Starting database init ...");
    core_->database_->init(*ioc_, core_);

    std::puts("Starting HTTP listener ...");
    boost::make_shared<copper::components::http_listener>(
        *ioc_, core_, server_id_, http_endpoint_)
        ->run();
    thread_ = std::thread([&] { ioc_->run(); });

    std::puts("Waiting for components to become ready ...");
    while (!core_->database_ready_ || !core_->http_ready_) {
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
  std::unique_ptr<boost::asio::io_context> ioc_;
  shared<core> core_;
  std::thread thread_;
};

TEST_F(HttpServerTestFixture, RootReturns404Json) {
  boost::asio::io_context client_ioc;
  boost::asio::ip::tcp::resolver resolver(client_ioc);
  auto results = resolver.resolve("127.0.0.1", "9009");

  boost::beast::tcp_stream stream(client_ioc);
  stream.connect(results);

  boost::beast::flat_buffer buffer;
  res response;
  req request{method::get, "/", 11};

  request.set(fields::host, "127.0.0.1");
  request.set(fields::user_agent, "Copper");

  boost::beast::http::write(stream, request);
  boost::beast::http::read(stream, buffer, response);

  ASSERT_EQ(response.body(), "{}");
  ASSERT_EQ(response.result_int(), 404);
  ASSERT_EQ(response[fields::content_type], "application/json");
  ASSERT_EQ(response[fields::access_control_allow_origin], "*");
  ASSERT_EQ(response["X-Server"], "Copper");
  ASSERT_TRUE(response.count("X-Time") > 0);

  boost::system::error_code ec;
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  stream.close();
}

TEST_F(HttpServerTestFixture, RootReturnsCompressed404Gzip) {
  boost::asio::io_context client_ioc;
  boost::asio::ip::tcp::resolver resolver(client_ioc);
  auto results = resolver.resolve("127.0.0.1", "9009");

  boost::beast::tcp_stream stream(client_ioc);
  stream.connect(results);

  boost::beast::flat_buffer buffer;
  res response;
  req request{method::get, "/", 11};

  request.set(fields::host, "127.0.0.1");
  request.set(fields::user_agent, "Copper");
  request.set(fields::accept_encoding, "gzip");

  boost::beast::http::write(stream, request);
  boost::beast::http::read(stream, buffer, response);

  ASSERT_EQ(gunzip_decompress(response.body()), "{}");
  ASSERT_EQ(response.result_int(), 404);
  ASSERT_EQ(response[fields::content_type], "application/json");
  ASSERT_EQ(response[fields::content_encoding], "gzip");
  ASSERT_EQ(response[fields::access_control_allow_origin], "*");
  ASSERT_EQ(response["X-Server"], "Copper");
  ASSERT_TRUE(response.count("X-Time") > 0);

  boost::system::error_code ec;
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  stream.close();
}

TEST_F(HttpServerTestFixture, RootReturnsHtml404) {
  boost::asio::io_context client_ioc;
  boost::asio::ip::tcp::resolver resolver(client_ioc);
  auto results = resolver.resolve("127.0.0.1", "9009");

  boost::beast::tcp_stream stream(client_ioc);
  stream.connect(results);

  boost::beast::flat_buffer buffer;
  res response;
  req request{method::get, "/", 11};

  request.set(fields::host, "127.0.0.1");
  request.set(fields::user_agent, "Copper");
  request.set(fields::accept, "text/html");
  request.set(fields::accept_encoding, "gzip");

  boost::beast::http::write(stream, request);
  boost::beast::http::read(stream, buffer, response);

  ASSERT_EQ(response.result_int(), 404);
  ASSERT_EQ(response[fields::content_type], "text/html");
  ASSERT_EQ(response[fields::content_encoding], "gzip");
  ASSERT_EQ(response[fields::access_control_allow_origin], "*");
  ASSERT_EQ(response["X-Server"], "Copper");
  ASSERT_TRUE(response.count("X-Time") > 0);
  ASSERT_TRUE(boost::starts_with(gunzip_decompress(response.body()),
                                 "<!doctype html>"));

  boost::system::error_code ec;
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  stream.close();
}

TEST_F(HttpServerTestFixture, RootReturnsHtmlNonCompressed404) {
  boost::asio::io_context client_ioc;
  boost::asio::ip::tcp::resolver resolver(client_ioc);
  auto results = resolver.resolve("127.0.0.1", "9009");

  boost::beast::tcp_stream stream(client_ioc);
  stream.connect(results);

  boost::beast::flat_buffer buffer;
  res response;
  req request{method::get, "/", 11};

  request.set(fields::host, "127.0.0.1");
  request.set(fields::user_agent, "Copper");
  request.set(fields::accept, "text/html");

  boost::beast::http::write(stream, request);
  boost::beast::http::read(stream, buffer, response);

  ASSERT_EQ(response.result_int(), 404);
  ASSERT_EQ(response[fields::content_type], "text/html");
  ASSERT_EQ(response[fields::access_control_allow_origin], "*");
  ASSERT_EQ(response["X-Server"], "Copper");
  ASSERT_TRUE(response.count("X-Time") > 0);
  ASSERT_TRUE(boost::starts_with(response.body(), "<!doctype html>"));

  boost::system::error_code ec;
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  stream.close();
}

TEST_F(HttpServerTestFixture, OptionsApiUpReturns200) {
  boost::asio::io_context client_ioc;
  boost::asio::ip::tcp::resolver resolver(client_ioc);
  auto results = resolver.resolve("127.0.0.1", "9009");

  boost::beast::tcp_stream stream(client_ioc);
  stream.connect(results);

  boost::beast::flat_buffer buffer;
  res response;
  req request{method::options, "/api/up", 11};

  request.set(fields::host, "127.0.0.1");
  request.set(fields::user_agent, "Copper");

  boost::beast::http::write(stream, request);
  boost::beast::http::read(stream, buffer, response);

  ASSERT_EQ(response.result_int(), 200);
  ASSERT_EQ(response[fields::content_type], "application/json");
  ASSERT_EQ(response[fields::access_control_allow_origin], "*");
  ASSERT_EQ(response[fields::access_control_allow_methods], "GET");
  ASSERT_EQ(response["X-Server"], "Copper");
  ASSERT_TRUE(response.count("X-Time") > 0);
  ASSERT_EQ(response.body(), "{}");

  boost::system::error_code ec;
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  stream.close();
}

TEST_F(HttpServerTestFixture, OptionsGzipApiUpReturns200) {
  boost::asio::io_context client_ioc;
  boost::asio::ip::tcp::resolver resolver(client_ioc);
  auto results = resolver.resolve("127.0.0.1", "9009");

  boost::beast::tcp_stream stream(client_ioc);
  stream.connect(results);

  boost::beast::flat_buffer buffer;
  res response;
  req request{method::options, "/api/up", 11};

  request.set(fields::host, "127.0.0.1");
  request.set(fields::user_agent, "Copper");
  request.set(fields::accept_encoding, "gzip");

  boost::beast::http::write(stream, request);
  boost::beast::http::read(stream, buffer, response);

  std::cout << response << std::endl;

  ASSERT_EQ(response.result_int(), 200);
  ASSERT_EQ(response[fields::content_type], "application/json");
  ASSERT_EQ(response[fields::content_encoding], "gzip");
  ASSERT_EQ(response[fields::access_control_allow_origin], "*");
  ASSERT_EQ(response[fields::access_control_allow_methods], "GET");
  ASSERT_EQ(response["X-Server"], "Copper");
  ASSERT_TRUE(response.count("X-Time") > 0);
  ASSERT_EQ(gunzip_decompress(response.body()), "{}");

  boost::system::error_code ec;
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  stream.close();
}

TEST_F(HttpServerTestFixture, ApiUpReturns429AfterRateLimit) {
  boost::asio::io_context client_ioc;
  boost::asio::ip::tcp::resolver resolver(client_ioc);
  auto results = resolver.resolve("127.0.0.1", "9009");

  boost::beast::tcp_stream stream(client_ioc);
  stream.connect(results);

  for (int i = 0; i < 5; ++i) {
    boost::beast::flat_buffer buffer;
    res response;
    req request{method::get, "/api/up", 11};

    request.set(fields::host, "127.0.0.1");
    request.set(fields::user_agent, "Copper");

    boost::beast::http::write(stream, request);
    boost::beast::http::read(stream, buffer, response);

    ASSERT_EQ(response.result_int(), 200);
    ASSERT_EQ(response[fields::content_type], "application/json");
    ASSERT_EQ(response[fields::access_control_allow_origin], "*");
    ASSERT_EQ(response["X-Server"], "Copper");
    ASSERT_TRUE(response.count("X-Time") > 0);
    ASSERT_TRUE(boost::starts_with(response.body(), R"({"timestamp":)"));
  }

  {
    boost::beast::flat_buffer buffer;
    res response;
    req request{method::get, "/api/up", 11};

    boost::beast::http::write(stream, request);
    boost::beast::http::read(stream, buffer, response);

    std::cout << response << std::endl;

    ASSERT_EQ(response.result_int(), 429);
    ASSERT_EQ(response.body(), "{}");
    ASSERT_TRUE(response.count("X-Rate-Until") > 0);
    ASSERT_TRUE(response.count("X-Server") > 0);
    ASSERT_TRUE(response.count("X-Time") > 0);
  }

  boost::system::error_code ec;
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  stream.close();
}

TEST_F(HttpServerTestFixture, ApiExceptionReturns500) {
  boost::asio::io_context client_ioc;
  boost::asio::ip::tcp::resolver resolver(client_ioc);
  auto results = resolver.resolve("127.0.0.1", "9009");

  boost::beast::tcp_stream stream(client_ioc);
  stream.connect(results);

  boost::beast::flat_buffer buffer;
  res response;
  req request{method::get, "/api/exception", 11};

  request.set(fields::host, "127.0.0.1");
  request.set(fields::user_agent, "Copper");

  boost::beast::http::write(stream, request);
  boost::beast::http::read(stream, buffer, response);

  std::cout << response << std::endl;

  ASSERT_EQ(response.result_int(), 500);
  ASSERT_EQ(response.body(), "{}");

  boost::system::error_code ec;
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  stream.close();
}

TEST_F(HttpServerTestFixture, ApiRegisterCreatesUser) {
  boost::asio::io_context client_ioc;
  boost::asio::ip::tcp::resolver resolver(client_ioc);
  auto results = resolver.resolve("127.0.0.1", "9009");

  boost::beast::tcp_stream stream(client_ioc);
  stream.connect(results);

  boost::beast::flat_buffer buffer;
  res response;
  req request{method::post, "/api/register", 11};

  request.set(fields::host, "127.0.0.1");
  request.set(fields::user_agent, "Copper");
  request.set(fields::content_type, "application/json");

  auto random_email = random_string(6) + "@zentrack.cl";

  boost::json::object new_user = {{"name", "Ian Torres"},
                                  {"email", random_email},
                                  {"password", "abc123abc"},
                                  {"password_confirmation", "abc123abc"}};

  request.body() = serialize(new_user);
  request.prepare_payload();

  boost::beast::http::write(stream, request);
  boost::beast::http::read(stream, buffer, response);

  std::cout << response << std::endl;

  ASSERT_EQ(response.result_int(), 200);
  ASSERT_TRUE(boost::contains(response.body(), "User created successfully."));

  boost::system::error_code ec;
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  stream.close();
}

TEST_F(HttpServerTestFixture, ApiAuthReturnsTokenAndAccessesUser) {
  boost::asio::io_context client_ioc;
  boost::asio::ip::tcp::resolver resolver(client_ioc);
  auto results = resolver.resolve("127.0.0.1", "9009");

  boost::beast::tcp_stream stream(client_ioc);
  stream.connect(results);

  boost::beast::flat_buffer buffer;
  res response;
  req request{method::post, "/api/auth", 11};

  request.set(fields::host, "127.0.0.1");
  request.set(fields::user_agent, "Copper");
  request.set(fields::content_type, "application/json");

  boost::json::object credentials = {{"email", "iantorres@outlook.com"},
                                     {"password", "abcdef"}};

  request.body() = serialize(credentials);
  request.prepare_payload();

  boost::beast::http::write(stream, request);
  boost::beast::http::read(stream, buffer, response);

  ASSERT_EQ(response.result_int(), 200);
  ASSERT_TRUE(boost::starts_with(response.body(), R"({"token":"Bearer)"));

  auto json_response = boost::json::parse(response.body());

  boost::beast::flat_buffer user_buffer;
  res user_response;
  req user_request{method::get, "/api/user", 11};

  user_request.set(fields::host, "127.0.0.1");
  user_request.set(fields::user_agent, "Copper");
  user_request.set(fields::authorization,
                   json_response.as_object().at("token").as_string());

  boost::beast::http::write(stream, user_request);
  boost::beast::http::read(stream, user_buffer, user_response);

  ASSERT_EQ(user_response.result_int(), 200);
  ASSERT_TRUE(boost::starts_with(user_response.body(), R"({"id":")"));

  boost::system::error_code ec;
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  stream.close();
}

TEST_F(HttpServerTestFixture, ApiAuthReturnsCompressedTokenAndAccessesUser) {
  boost::asio::io_context client_ioc;
  boost::asio::ip::tcp::resolver resolver(client_ioc);
  auto results = resolver.resolve("127.0.0.1", "9009");

  boost::beast::tcp_stream stream(client_ioc);
  stream.connect(results);

  boost::beast::flat_buffer buffer;
  res response;
  req request{method::post, "/api/auth", 11};

  request.set(fields::host, "127.0.0.1");
  request.set(fields::user_agent, "Copper");
  request.set(fields::content_type, "application/json");
  request.set(fields::accept_encoding, "gzip");

  boost::json::object credentials = {{"email", "iantorres@outlook.com"},
                                     {"password", "abcdef"}};

  request.body() = serialize(credentials);
  request.prepare_payload();

  boost::beast::http::write(stream, request);
  boost::beast::http::read(stream, buffer, response);

  ASSERT_EQ(response.result_int(), 200);
  ASSERT_TRUE(boost::starts_with(gunzip_decompress(response.body()),
                                 R"({"token":"Bearer)"));

  auto json_response = boost::json::parse(gunzip_decompress(response.body()));

  boost::beast::flat_buffer user_buffer;
  res user_response;
  req user_request{method::get, "/api/user", 11};

  user_request.set(fields::host, "127.0.0.1");
  user_request.set(fields::user_agent, "Copper");
  user_request.set(fields::accept_encoding, "gzip");
  user_request.set(fields::authorization,
                   json_response.as_object().at("token").as_string());

  boost::beast::http::write(stream, user_request);
  boost::beast::http::read(stream, user_buffer, user_response);

  ASSERT_EQ(user_response.result_int(), 200);
  ASSERT_TRUE(boost::starts_with(gunzip_decompress(user_response.body()),
                                 R"({"id":")"));

  boost::beast::flat_buffer unauthorized_user_buffer;
  res unauthorized_user_response;
  req unauthorized_user_request{method::get, "/api/user", 11};

  unauthorized_user_request.set(fields::host, "127.0.0.1");
  unauthorized_user_request.set(fields::user_agent, "Copper");
  unauthorized_user_request.set(fields::authorization, "Bearer invalid");

  boost::beast::http::write(stream, unauthorized_user_request);
  boost::beast::http::read(stream, unauthorized_user_buffer, user_response);

  ASSERT_EQ(user_response.result_int(), 401);

  boost::system::error_code ec;
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  stream.close();
}

TEST_F(HttpServerTestFixture, ApiParamsWithDynamicBinding) {
  boost::asio::io_context client_ioc;
  boost::asio::ip::tcp::resolver resolver(client_ioc);
  auto results = resolver.resolve("127.0.0.1", "9009");

  boost::beast::tcp_stream stream(client_ioc);
  stream.connect(results);

  boost::beast::flat_buffer buffer;
  res response;
  req request{method::get, "/api/params/hello", 11};

  request.set(fields::host, "127.0.0.1");
  request.set(fields::user_agent, "Copper");

  boost::beast::http::write(stream, request);
  boost::beast::http::read(stream, buffer, response);

  ASSERT_EQ(response.result_int(), 200);
  ASSERT_TRUE(boost::contains(response.body(), "hello"));

  boost::system::error_code ec;
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  stream.close();
}

TEST_F(HttpServerTestFixture, ApiParamsWithQuerystrings) {
  boost::asio::io_context client_ioc;
  boost::asio::ip::tcp::resolver resolver(client_ioc);
  auto results = resolver.resolve("127.0.0.1", "9009");

  boost::beast::tcp_stream stream(client_ioc);
  stream.connect(results);

  boost::beast::flat_buffer buffer;
  res response;
  req request{method::get, "/api/params/hello?a=b&c=d&e[]=f&e[]=g&z", 11};

  request.set(fields::host, "127.0.0.1");
  request.set(fields::user_agent, "Copper");

  boost::beast::http::write(stream, request);
  boost::beast::http::read(stream, buffer, response);

  ASSERT_EQ(response.result_int(), 200);
  ASSERT_TRUE(boost::contains(response.body(), "hello"));

  boost::system::error_code ec;
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  stream.close();
}

TEST_F(HttpServerTestFixture, BadRequestPathTraversalReturns400) {
  boost::asio::io_context client_ioc;
  boost::asio::ip::tcp::resolver resolver(client_ioc);
  auto results = resolver.resolve("127.0.0.1", "9009");

  boost::beast::tcp_stream stream(client_ioc);
  stream.connect(results);

  boost::beast::flat_buffer buffer;
  res response;
  req request{method::get, "/api/../bad_request", 11};

  request.set(fields::host, "127.0.0.1");
  request.set(fields::user_agent, "Copper");

  boost::beast::http::write(stream, request);
  boost::beast::http::read(stream, buffer, response);

  ASSERT_EQ(response.result_int(), 400);
  ASSERT_EQ(response.body(), "{}");

  boost::system::error_code ec;
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  stream.close();
}