#include <copper/app.hpp>

#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include <copper/components/certificates.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/task_group.hpp>
#include <copper/components/signal_handler.hpp>
#include <copper/components/state.hpp>
#include <copper/components/shared.hpp>
#include <boost/redis/connection.hpp>

namespace copper {

    std::string get_version() {
        return "2.0.0";
    }

    // LCOV_EXCL_START
    void run() {
        dotenv::init();

        auto const address = boost::asio::ip::make_address(dotenv::getenv("APP_HOST", "0.0.0.0"));
        auto const port = (unsigned short) std::stoi(dotenv::getenv("APP_PORT", "9000"));
        auto const endpoint = boost::asio::ip::tcp::endpoint { address, port };
        auto const doc_root = std::string_view{ "."};
        auto const threads = std::max<int>(1, std::stoi(dotenv::getenv("APP_THREADS", "1")));

        boost::asio::io_context ioc{threads};

        boost::asio::ssl::context ctx{ boost::asio::ssl::context::tlsv12 };

        load_server_certificate(ctx);

        auto task_group = boost::make_shared<components::task_group>(ioc.get_executor());

        auto state = boost::make_shared<components::state>();

        boost::asio::co_spawn(
                boost::asio::make_strand(ioc),
                components::listener(state, task_group, ctx, endpoint, doc_root),
                task_group->adapt(
                        [](std::exception_ptr e)
                        {
                            if(e)
                            {
                                try
                                {
                                    std::rethrow_exception(e);
                                }
                                catch(std::exception& e)
                                {
                                    std::cerr << "Error in listener: " << e.what() << "\n";
                                }
                            }
                        }));

        boost::asio::co_spawn(boost::asio::make_strand(ioc), components::signal_handler(task_group), boost::asio::detached);

        std::vector<std::thread> v;
        v.reserve(threads - 1);
        for(auto i = threads - 1; i > 0; --i)
            v.emplace_back([&ioc] { ioc.run(); });
        ioc.run();

        for(auto& t : v)
            t.join();
    }
    // LCOV_EXCL_STOP
}