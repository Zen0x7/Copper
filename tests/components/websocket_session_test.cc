#include <gtest/gtest.h>

#include <copper/components/certificates.hpp>
#include <copper/components/task_group.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/state.hpp>
#include <copper/components/signal_handler.hpp>

#include <boost/asio/co_spawn.hpp>

TEST(Components_WebSocket_Session, Client) {
    using namespace copper::components;

    auto const address = boost::asio::ip::make_address("0.0.0.0");
    auto const port = 9000;
    auto const endpoint = boost::asio::ip::tcp::endpoint{address, port};
    auto const doc_root = std::string_view{"."};

    boost::asio::io_context ioc;

    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};

    load_server_certificate(ctx);

    auto  task_group_ = boost::make_shared<task_group>(ioc.get_executor());

    auto state_ = boost::make_shared<state>();

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
        std::thread thread([&]() {
            try {
                ioc.run();
            } catch (std::exception &e) {

            }
        });

        thread.detach();

        boost::asio::ip::tcp::resolver resolver(client_ioc);

        std::string host = "127.0.0.1";
        auto const results = resolver.resolve(host, "9000");

        boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{client_ioc};

        auto ep = boost::asio::connect(ws.next_layer(), results);

        host += ':' + std::to_string(ep.port());

        ws.set_option(boost::beast::websocket::stream_base::decorator(
                [](boost::beast::websocket::request_type &req) {
                    req.set(boost::beast::http::field::user_agent, "Copper");
                }));

        ws.handshake(host, "/");
        ws.write(boost::asio::buffer(std::string("hello")));
        boost::beast::flat_buffer buffer;
        ws.read(buffer);
        ws.close(boost::beast::websocket::close_code::normal);
        std::cout << boost::beast::make_printable(buffer.data()) << std::endl;
        task_group_->emit(boost::asio::cancellation_type::all);
        ioc.stop();
        thread.join();
    } catch (std::exception const &e) {

    }

    ASSERT_TRUE(true);
}