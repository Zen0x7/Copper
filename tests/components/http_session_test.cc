#include <gtest/gtest.h>

#include <copper/components/certificates.hpp>
#include <copper/components/task_group.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/signal_handler.hpp>


TEST(Components_TCP_Listener, Client) {
    using namespace copper::components;

    auto const address = boost::asio::ip::make_address("0.0.0.0");
    auto const port = 9001;
    auto const endpoint = boost::asio::ip::tcp::endpoint{address, port};
    auto const doc_root = std::string_view{"."};

    boost::asio::io_context ioc;

    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};

    load_server_certificate(ctx);

    task_group task_group{ioc.get_executor()};

    boost::asio::co_spawn(
            boost::asio::make_strand(ioc),
            listener(task_group, ctx, endpoint, doc_root),
            task_group.adapt(
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

    boost::asio::co_spawn(boost::asio::make_strand(ioc), signal_handler(task_group), boost::asio::detached);

    boost::asio::io_context client_ioc;


    std::thread thread([&]() {
        ioc.run();
    });

    thread.detach();

    sleep(1);

    boost::asio::ip::tcp::resolver resolver(client_ioc);
    boost::beast::tcp_stream stream(client_ioc);

    auto const host = "0.0.0.0";
    auto const results = resolver.resolve(host, "9001");
    stream.connect(results);

    boost::beast::http::request<boost::beast::http::string_body> req{boost::beast::http::verb::get, "/", 11};
    req.set(boost::beast::http::field::host, host);
    req.set(boost::beast::http::field::user_agent, "Copper");

    boost::beast::http::request<boost::beast::http::string_body> close_req{boost::beast::http::verb::get, "/", 11};
    close_req.set(boost::beast::http::field::host, host);
    close_req.set(boost::beast::http::field::user_agent, "Copper");
    close_req.set(boost::beast::http::field::connection, "close");

    boost::beast::flat_buffer buffer;
    boost::beast::http::response<boost::beast::http::dynamic_body> res;

    boost::beast::http::write(stream, req);
    boost::beast::http::read(stream, buffer, res);
    boost::beast::http::write(stream, req);
    boost::beast::http::read(stream, buffer, res);
    boost::beast::http::write(stream, close_req);
    boost::beast::http::read(stream, buffer, res);

    try {
        task_group.emit(boost::asio::cancellation_type::all);

        thread.join();
    } catch (...) {

    }

    ASSERT_TRUE(true);
}