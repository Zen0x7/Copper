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

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/program_options.hpp>
#include <boost/uuid/random_generator.hpp>
#include <copper/app.hpp>
#include <copper/components/base64.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/controllers/api/auth_controller.hpp>
#include <copper/components/controllers/api/connections_controller.hpp>
#include <copper/components/controllers/api/register_controller.hpp>
#include <copper/components/controllers/api/up_controller.hpp>
#include <copper/components/controllers/api/user_controller.hpp>
#include <copper/components/core.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/http_listener.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/router.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/signal_handler.hpp>
#include <copper/components/subscriber.hpp>
#include <copper/components/tcp_listener.hpp>
#include <copper/components/websocket_listener.hpp>
#include <iostream>
#include <thread>

namespace copper {
std::string get_version() {
  return "1.0.0";
}

// LCOV_EXCL_START
int run(int argc, const char* argv[]) {
  using namespace components;

  auto _server_id = boost::uuids::random_generator()();

  boost::program_options::options_description _program_description(
      "Allowed options");

  _program_description.add_options()("help", "Get more details about options")(
      "as",
      boost::program_options::value<std::string>()->default_value("service"),
      "Run as `service` or `command`.")(
      "command",
      boost::program_options::value<std::string>()->default_value("none"),
      "The `command` name.");

  boost::program_options::options_description _command_invoke_description(
      "Command `invoke` options");

  _command_invoke_description.add_options()(
      "method",
      boost::program_options::value<std::string>()->default_value("GET"),
      "HTTP verb.")(
      "signature",
      boost::program_options::value<std::string>()->default_value("/"),
      "HTTP `path`.")(
      "headers",
      boost::program_options::value<std::string>()->default_value("{}"),
      "HTTP `headers`.")(
      "body", boost::program_options::value<std::string>()->default_value("{}"),
      "HTTP `body`.");

  boost::program_options::options_description _commandline_description;
  _commandline_description.add(_program_description);
  _commandline_description.add(_command_invoke_description);

  boost::program_options::variables_map _vm;
  store(parse_command_line(argc, argv, _commandline_description), _vm);

  if (_vm.contains("help")) {
    LOG(_commandline_description);
    return EXIT_SUCCESS;
  }

  boost::asio::io_context _ioc{1};

  auto _core = core::factory(_ioc);

  _core->views_->push("404", "404");

  const auto _as = _vm["as"].as<std::string>();

  auto const _address =
      boost::asio::ip::make_address(_core->configuration_->get()->app_host_);
  auto const _http_port =
      static_cast<unsigned short>(_core->configuration_->get()->app_port_);
  auto const _tcp_port =
      static_cast<unsigned short>(_core->configuration_->get()->tcp_port_);
  auto const _websocket_port = static_cast<unsigned short>(
      _core->configuration_->get()->websocket_port_);

  auto const _http_endpoint =
      boost::asio::ip::tcp::endpoint{_address, _http_port};
  auto const _tcp_endpoint =
      boost::asio::ip::tcp::endpoint{_address, _tcp_port};
  auto const _websocket_endpoint =
      boost::asio::ip::tcp::endpoint{_address, _websocket_port};

  auto const _threads =
      std::max<int>(1, _core->configuration_->get()->app_threads_);

  _core->router_
      ->push(method::get, "/api/connections",
             boost::make_shared<controllers::api::connections_controller>(),
             {.use_auth_ = false, .use_throttler_ = false})
      ->push(method::get, "/api/user",
             boost::make_shared<controllers::api::user_controller>(),
             {
                 .use_auth_ = true,
                 .use_throttler_ = true,
                 .rpm_ = 5,
             })
      ->push(method::get, "/api/up",
             boost::make_shared<controllers::api::up_controller>(),
             {
                 .use_throttler_ = true,
                 .rpm_ = 5,
             })
      ->push(method::post, "/api/auth",
             boost::make_shared<controllers::api::auth_controller>(),
             {
                 .use_throttler_ = true,
                 .use_validator_ = true,
                 .rpm_ = 5,
             })
      ->push(method::post, "/api/register",
             boost::make_shared<controllers::api::register_controller>(),
             {
                 .use_throttler_ = true,
                 .use_validator_ = true,
                 .rpm_ = 5,
             });

  if (_as == "service") {
    signal_handler([_core, &_ioc](int /*sig*/) {
      std::puts("Service is being stopped ... ");
      _core->database_->stop();
      _ioc.stop();
      std::puts("Service has been stopped ... ");
    });

    _core->database_->init(_ioc, _core);
    boost::make_shared<http_listener>(_ioc, _core, _server_id, _http_endpoint)
        ->run();
    boost::make_shared<tcp_listener>(_ioc, _core, _server_id, _tcp_endpoint)
        ->run();
    boost::make_shared<websocket_listener>(_ioc, _core, _server_id,
                                           _websocket_endpoint)
        ->run();
    // co_spawn(_ioc, tcp_listener(_core, _server_id, _tcp_endpoint),
    // boost::asio::detached);
    // co_spawn(_ioc, http_listener(_core, _server_id, _http_endpoint),
    // boost::asio::detached);
    // co_spawn(_ioc, websocket_listener(_core, _server_id,
    // _websocket_endpoint), boost::asio::detached);

    // subscriber::start(_core, _server_id);

    vector_of<std::thread> _threads_container;
    _threads_container.reserve(_threads - 1);
    for (auto _i = _threads - 1; _i > 0; --_i)
      _threads_container.emplace_back([&_ioc] { _ioc.run(); });
    _ioc.run();

    for (auto& _thead : _threads_container)
      _thead.join();

  } else {
    if (const auto _command = _vm["command"].as<std::string>();
        _command == "keygen") {
      auto [_key, _iv] = cipher_generate_aes_key_iv();

      fmt::print("APP_KEY={}\n", base64_encode(_key));
      fmt::print("APP_KEY_IV={}\n", base64_encode(_iv));
    }
  }

  return EXIT_SUCCESS;
}
// LCOV_EXCL_STOP
}  // namespace copper
