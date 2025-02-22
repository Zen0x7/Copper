#include <fmt/core.h>

#include <boost/asio/co_spawn.hpp>
#include <boost/program_options.hpp>
#include <copper/app.hpp>
#include <copper/components/base64.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/server_certificates.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/signal_handler.hpp>
#include <copper/components/state.hpp>
#include <copper/components/task_group.hpp>
#include <copper/controllers/auth_controller.hpp>
#include <copper/controllers/up_controller.hpp>
#include <copper/controllers/user_controller.hpp>
#include <thread>

namespace copper {

std::string get_version() { return "3.0.0"; }

// LCOV_EXCL_START
int run(int argc, const char *argv[]) {
  using namespace components;

  dotenv::init();
  boost::program_options::options_description program_description(
      "Allowed options");

  program_description.add_options()("help", "Get more details about options")(
      "as",
      boost::program_options::value<std::string>()->default_value("service"),
      "Run as `service` or `command`.")(
      "command",
      boost::program_options::value<std::string>()->default_value("none"),
      "The `command` name.");

  boost::program_options::options_description commandline_description;
  commandline_description.add(program_description);

  boost::program_options::variables_map vm;
  store(parse_command_line(argc, argv, commandline_description), vm);

  if (vm.contains("help")) {
    //    std::cout << commandline_description << std::endl;
    return EXIT_FAILURE;
  }

  const auto as = vm["as"].as<std::string>();

  if (as == "service") {
    auto const address =
        boost::asio::ip::make_address(dotenv::getenv("APP_HOST", "0.0.0.0"));
    auto const port =
        (unsigned short)std::stoi(dotenv::getenv("APP_PORT", "9000"));
    auto const endpoint = boost::asio::ip::tcp::endpoint{address, port};
    auto const doc_root = std::string_view{"."};
    auto const threads =
        std::max<int>(1, std::stoi(dotenv::getenv("APP_THREADS", "8")));

    boost::asio::io_context ioc{threads};

    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};

    ctx.set_options(boost::asio::ssl::context::default_workarounds |
                    boost::asio::ssl::context::single_dh_use);
    ctx.use_certificate_chain_file(
        dotenv::getenv("APP_PUBLIC_KEY", "./certificates/public.pem"));
    ctx.use_private_key_file(
        dotenv::getenv("APP_PRIVATE_KEY", "./certificates/private.pem"),
        boost::asio::ssl::context::pem);
    ctx.use_tmp_dh_file(
        dotenv::getenv("APP_DH_PARAMS", "./certificates/params.pem"));

    auto _task_group = boost::make_shared<task_group>(ioc.get_executor());

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

    auto _state = boost::make_shared<state>(database_pool);

    _state->get_database()->start();

    _state->get_http_router()
        ->push(http_method::get, "/api/user",
               boost::make_shared<controllers::user_controller>(),
               {
                   .use_auth_ = true,
                   .use_throttler_ = true,
                   .rpm_ = 5,
               })
        ->push(http_method::get, "/api/up",
               boost::make_shared<controllers::up_controller>(),
               {
                   .use_throttler_ = true,
                   .rpm_ = 5,
               })
        ->push(http_method::post, "/api/auth",
               boost::make_shared<controllers::auth_controller>(),
               {
                   .use_throttler_ = true,
                   .use_validator_ = true,
                   .rpm_ = 5,
               });

    boost::asio::co_spawn(
        boost::asio::make_strand(ioc),
        listener(_state, _task_group, ctx, endpoint, doc_root),
        _task_group->adapt([](std::exception_ptr e) {
          if (e) {
            try {
              std::rethrow_exception(e);
            } catch (std::exception &e) {
              //              std::cerr << "Error in listener: " << e.what() <<
              //              "\n";
            }
          }
        }));

    boost::asio::co_spawn(boost::asio::make_strand(ioc),
                          signal_handler(_task_group), boost::asio::detached);

    containers::vector_of<std::thread> v;
    v.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i)
      v.emplace_back([&ioc] { ioc.run(); });
    ioc.run();

    for (auto &t : v) t.join();

    return EXIT_SUCCESS;
  } else {
    const auto command = vm["command"].as<std::string>();

    if (command == "keygen") {
      auto key = cipher_generate_aes_key_iv();

      fmt::print("APP_KEY={}\n", base64_encode(key.first));
      fmt::print("APP_KEY_IV={}\n", base64_encode(key.second));
    }

    return EXIT_SUCCESS;
  }
}
// LCOV_EXCL_STOP
}  // namespace copper