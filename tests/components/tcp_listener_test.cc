#include <gtest/gtest.h>

#include <copper/components/tcp_listener.hpp>


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
    req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Send the HTTP request to the remote host
    boost::beast::http::write(stream, req);

    // This buffer is used for reading and must be persisted
    boost::beast::flat_buffer buffer;

    // Declare a container to hold the response
    boost::beast::http::response<boost::beast::http::dynamic_body> res;

    // Receive the HTTP response
    boost::beast::http::read(stream, buffer, res);

    // Write the message to standard out
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
