#include <sentry.h>

#include <boost/asio/co_spawn.hpp>
#include <boost/program_options.hpp>
#include <copper/app.hpp>
#include <copper/components/base64.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/logger.hpp>
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
#include <iostream>
#include <thread>

namespace copper {

std::string get_version() { return "6.0.0"; }

// LCOV_EXCL_START
int run(int argc, const char *argv[]) {
  using namespace components;

  auto _server_id = boost::uuids::random_generator()();

  spdlog::flush_on(spdlog::level::info);

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
    std::cout << _commandline_description << std::endl;
    return EXIT_SUCCESS;
  }

  auto _configuration = boost::make_shared<configuration>();

  sentry_options_t *_options = sentry_options_new();
  sentry_options_set_dsn(_options, _configuration->get()->sentry_dsn_.c_str());
  sentry_options_set_database_path(_options, ".sentry-native");
  sentry_options_set_handler_path(
      _options, _configuration->get()->sentry_crashpad_handler_.c_str());
  std::string _release = "copper@" + get_version();
  sentry_options_set_release(_options, _release.c_str());
  sentry_options_set_debug(_options,
                           _configuration->get()->app_debug_ == true ? 1 : 0);
  sentry_init(_options);

  const auto _as = _vm["as"].as<std::string>();

  auto const _address =
      boost::asio::ip::make_address(_configuration->get()->app_host_);
  auto const _port =
      static_cast<unsigned short>(_configuration->get()->app_port_);
  auto const _endpoint = boost::asio::ip::tcp::endpoint{_address, _port};
  constexpr auto _doc_root = std::string_view{"."};
  auto const _threads = std::max<int>(1, _configuration->get()->app_threads_);

  boost::asio::io_context _ioc{_threads};

  boost::asio::ssl::context _ctx{boost::asio::ssl::context::tlsv12};

  _ctx.set_options(boost::asio::ssl::context::default_workarounds |
                   boost::asio::ssl::context::single_dh_use);
  _ctx.use_certificate_chain_file(_configuration->get()->app_public_key_);
  _ctx.use_private_key_file(_configuration->get()->app_private_key_,
                            boost::asio::ssl::context::pem);
  _ctx.use_tmp_dh_file(_configuration->get()->app_dh_params_);

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

  auto _state = boost::make_shared<state>(_database_pool);

  if (_as == "service") {
    logger::instance()->system_->info(
        "[{}] Server is running on [{}:{}]", to_string(_server_id),
        _configuration->get()->app_host_, _configuration->get()->app_port_);

    _state->get_database()->start();
  }

  _state->get_router()
      ->push(method::get, "/api/user",
             boost::make_shared<controllers::api::user_controller>(_state),
             {
                 .use_auth_ = true,
                 .use_throttler_ = true,
                 .rpm_ = 5,
             })
      ->push(method::get, "/api/up",
             boost::make_shared<controllers::api::up_controller>(_state),
             {
                 .use_throttler_ = true,
                 .rpm_ = 5,
             })
      ->push(method::post, "/api/auth",
             boost::make_shared<controllers::api::auth_controller>(_state),
             {
                 .use_throttler_ = true,
                 .use_validator_ = true,
                 .rpm_ = 5,
             });

  if (_as == "service") {
    co_spawn(
        make_strand(_ioc),
        listener(_server_id, _state, _task_group, _endpoint, _doc_root),
        _task_group->adapt([_state, _server_id](const std::exception_ptr &e) {
          if (e) {
            try {
              std::rethrow_exception(e);
            } catch (std::exception &exception) {
              logger::instance()->system_->info(
                  "[{}] Something went wrong: [{}] on [{}]",
                  to_string(_server_id), exception.what(), "listener");
            }
          }
        }));

    co_spawn(
        make_strand(_ioc), subscriber(),
        _task_group->adapt([_state, _server_id](const std::exception_ptr &e) {
          if (e) {
            try {
              std::rethrow_exception(e);
            } catch (std::exception &exception) {
              logger::instance()->system_->info(
                  "[{}] Something went wrong: [{}] on [{}]",
                  to_string(_server_id), exception.what(), "subscriber");
            }
          }
        }));

    co_spawn(make_strand(_ioc), signal_handler(_task_group),
             boost::asio::detached);

    containers::vector_of<std::thread> _threads_container;
    _threads_container.reserve(_threads - 1);
    for (auto _i = _threads - 1; _i > 0; --_i)
      _threads_container.emplace_back([&_ioc] { _ioc.run(); });
    _ioc.run();

    for (auto &t : _threads_container) t.join();

    logger::instance()->system_->info("[{}] Server has been shutdown",
                                      to_string(_server_id));
  } else {
    if (const auto _command = _vm["command"].as<std::string>();
        _command == "keygen") {
      auto [_key, _iv] = cipher_generate_aes_key_iv();

      fmt::print("APP_KEY={}\n", base64_encode(_key));
      fmt::print("APP_KEY_IV={}\n", base64_encode(_iv));
    } else if (_command == "invoke") {
      auto _method = _vm["method"].as<std::string>();
      auto _signature = _vm["signature"].as<std::string>();
      auto _headers = _vm["headers"].as<std::string>();
      auto _body = _vm["body"].as<std::string>();

      auto _verb = boost::beast::http::string_to_verb(_method);

      boost::asio::io_context _client_ioc;
      boost::asio::ip::tcp::resolver _resolver(_client_ioc);
      boost::beast::tcp_stream _stream(_client_ioc);

      auto const _results =
          _resolver.resolve(_configuration->get()->app_host_,
                            std::to_string(_configuration->get()->app_port_));

      _stream.connect(_results);

      {
        boost::beast::flat_buffer _buffer;
        boost::beast::http::response<boost::beast::http::string_body> _response;
        request _request{_verb, _signature, 11};
        _request.set(fields::host, _configuration->get()->app_host_);
        _request.set(fields::user_agent, "Copper");
        _request.set(fields::content_type, "application/json");
        _request.body() = _body;
        _request.prepare_payload();
        boost::beast::http::write(_stream, _request);
        boost::beast::http::read(_stream, _buffer, _response);

        std::cout << _response << std::endl;

        _buffer.clear();
        _response.clear();
      }

      boost::system::error_code _ec;
      _stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both,
                                _ec);
      if (_ec && _ec != boost::system::errc::not_connected) {
      }
      _stream.close();
      _client_ioc.run();
    }
  }

  sentry_close();
  return EXIT_SUCCESS;
}
// LCOV_EXCL_STOP
}  // namespace copper