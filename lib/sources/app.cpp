#include <copper/app.hpp>

#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include <copper/components/certificates.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/task_group.hpp>
#include <copper/components/signal_handler.hpp>
#include <copper/components/state.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/base64.hpp>
#include <boost/redis/connection.hpp>
#include <boost/asio/thread_pool.hpp>

#include <boost/program_options.hpp>
#include <boost/mysql/pool_params.hpp>
#include <boost/mysql/connection_pool.hpp>

#include <fmt/core.h>

#include <app/controllers/auth_controller.hpp>
#include <app/controllers/up_controller.hpp>
#include <app/controllers/user_controller.hpp>


namespace copper {

    std::string get_version() {
      return "2.0.0";
    }

    // LCOV_EXCL_START
    int run(int argc, const char *argv[]) {
      dotenv::init();
      boost::program_options::options_description program_description("Allowed options");

      program_description.add_options()
        ("help", "Get more details about options")
        ("as", boost::program_options::value<std::string>()->default_value("service"),
         "Run as `service` or `command`.")
        ("command", boost::program_options::value<std::string>()->default_value("none"), "The `command` name.");

      boost::program_options::options_description commandline_description;
      commandline_description.add(program_description);

      boost::program_options::variables_map vm;
      store(parse_command_line(argc, argv, commandline_description), vm);

      if (vm.contains("help")) {
        std::cout << commandline_description << std::endl;
        return EXIT_FAILURE;
      }

      const auto as = vm["as"].as<std::string>();

      if (as == "service") {
        auto const address = boost::asio::ip::make_address(dotenv::getenv("APP_HOST", "0.0.0.0"));
        auto const port = (unsigned short) std::stoi(dotenv::getenv("APP_PORT", "9000"));
        auto const endpoint = boost::asio::ip::tcp::endpoint{address, port};
        auto const doc_root = std::string_view{"."};
        auto const threads = std::max<int>(1, std::stoi(dotenv::getenv("APP_THREADS", "1")));

        boost::asio::io_context ioc{threads};

        boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};

        ctx.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::single_dh_use);
        ctx.use_certificate_chain_file(dotenv::getenv("APP_PUBLIC_KEY", "./certificates/public.pem"));
        ctx.use_private_key_file(dotenv::getenv("APP_PRIVATE_KEY", "./certificates/private.pem"),
                                 boost::asio::ssl::context::pem);
        ctx.use_tmp_dh_file(dotenv::getenv("APP_DH_PARAMS", "./certificates/params.pem"));

        auto task_group = boost::make_shared<components::task_group>(ioc.get_executor());

        auto state = boost::make_shared<components::state>();

        state->get_database()->start();

        state
          ->get_router()
          ->push(components::http_method::get, "/api/user", boost::make_shared<app::controllers::user_controller>(), {
            .use_auth = true,
            .use_throttler = true,
            .rpm = 5,
          })
          ->push(components::http_method::get, "/api/up", boost::make_shared<app::controllers::up_controller>(), {
            .use_throttler = true,
            .rpm = 5,
          })
          ->push(components::http_method::post, "/api/auth", boost::make_shared<app::controllers::auth_controller>(), {
            .use_throttler = true,
            .use_validator = true,
            .rpm = 5,
          });

        boost::asio::co_spawn(
          boost::asio::make_strand(ioc),
          components::listener(state, task_group, ctx, endpoint, doc_root),
          task_group->adapt(
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

        boost::asio::co_spawn(boost::asio::make_strand(ioc), components::signal_handler(task_group),
                              boost::asio::detached);

        std::vector<std::thread> v;
        v.reserve(threads - 1);
        for (auto i = threads - 1; i > 0; --i)
          v.emplace_back([&ioc] { ioc.run(); });
        ioc.run();

        for (auto &t: v)
          t.join();

        return EXIT_SUCCESS;
      } else {

        const auto command = vm["command"].as<std::string>();

        if (command == "keygen") {
          auto key = components::cipher_generate_aes_key_iv();

          fmt::print("APP_KEY={}\n", components::base64_encode(key.first));
          fmt::print("APP_KEY_IV={}\n", components::base64_encode(key.second));
        }

        return EXIT_SUCCESS;
      }
    }
    // LCOV_EXCL_STOP
}