#include <gtest/gtest.h>

#include <copper/components/tcp_listener.hpp>


TEST(Components_TCP_Listener, VolatileClient) {
using namespace copper::components;

    auto const address = boost::asio::ip::make_address("0.0.0.0");
    auto const port = 9000;
    auto const doc_root = boost::make_shared<std::string>(".");
    auto const threads = 1;

    boost::asio::io_context ioc{threads};
    boost::asio::io_context client_ioc{threads};

    auto listener = boost::make_shared<tcp_listener>(ioc, boost::asio::ip::tcp::endpoint {address, port }, doc_root);

    listener->run();

    std::thread thread([&]() {
        ioc.run();
    });

    thread.detach();

    sleep(1);

    boost::asio::ip::tcp::resolver resolver(client_ioc);
    boost::beast::tcp_stream stream(client_ioc);

    auto const host = "0.0.0.0";
    auto const results = resolver.resolve(host, "9000");
    stream.connect(results);

    boost::beast::http::request<boost::beast::http::string_body> req{boost::beast::http::verb::get, "/", 11};
    req.set(boost::beast::http::field::host, host);
    req.set(boost::beast::http::field::user_agent, "Copper");
    req.set(boost::beast::http::field::connection, "close");

    boost::beast::http::write(stream, req);
    boost::beast::flat_buffer buffer;

    boost::beast::http::response<boost::beast::http::dynamic_body> res;

    boost::beast::http::read(stream, buffer, res);

    std::cout << res << std::endl;

    boost::beast::error_code ec;
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);


    try {
        ioc.stop();
        thread.join();
    } catch (std::exception &e) {

    }

    ASSERT_TRUE(true);
}

TEST(Components_TCP_Listener, Client) {
    using namespace copper::components;

    auto const address = boost::asio::ip::make_address("0.0.0.0");
    auto const port = 9000;
    auto const doc_root = boost::make_shared<std::string>(".");
    auto const threads = 1;

    boost::asio::io_context ioc{threads};
    boost::asio::io_context client_ioc{threads};

    auto listener = boost::make_shared<tcp_listener>(ioc, boost::asio::ip::tcp::endpoint {address, port }, doc_root);

    listener->run();

    std::thread thread([&]() {
        ioc.run();
    });

    thread.detach();

    sleep(1);

    boost::asio::ip::tcp::resolver resolver(client_ioc);
    boost::beast::tcp_stream stream(client_ioc);

    auto const host = "0.0.0.0";
    auto const results = resolver.resolve(host, "9000");
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

    boost::beast::error_code ec;
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);


    try {
        ioc.stop();
        thread.join();
    } catch (std::exception &e) {

    }

    ASSERT_TRUE(true);
}