#include <copper/app.hpp>

#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include <copper/components/tcp_listener.hpp>

namespace copper {
    std::string get_version() {
        return "1.0.0";
    }

    void run() {
        auto const address = boost::asio::ip::make_address("0.0.0.0");
        auto const port = 9000;
        auto const doc_root = boost::make_shared<std::string>(".");
        auto const threads = std::max<int>(1, 4);

        boost::asio::io_context ioc{threads};

        auto listener = boost::make_shared<components::tcp_listener>(ioc, boost::asio::ip::tcp::endpoint {address, port }, doc_root);

        listener->run();

        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);

        signals.async_wait([&](boost::beast::error_code const &, int) {
            ioc.stop();
        });

        std::vector<std::thread> v;
        v.reserve(threads - 1);
        for(auto i = threads - 1; i > 0; --i)
            v.emplace_back(
                    [&ioc]
                    {
                        ioc.run();
                    });
        ioc.run();

        for(auto& t : v)
            t.join();
    }
}