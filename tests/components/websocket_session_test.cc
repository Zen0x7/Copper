#include <gtest/gtest.h>

#include <copper/components/tcp_listener.hpp>


TEST(Components_WebSocket_Session, Client) {
    using namespace copper::components;

    auto const address = boost::asio::ip::make_address("0.0.0.0");
    auto const port = 9000;
    auto const doc_root = boost::make_shared<std::string>(".");
    auto const threads = 1;

    boost::asio::io_context ioc{threads};
    boost::asio::io_context client_ioc{threads};

    auto listener = boost::make_shared<tcp_listener>(ioc, boost::asio::ip::tcp::endpoint{address, port}, doc_root);

    listener->run();

    std::thread thread([&]() {
        ioc.run();
    });

    thread.detach();

    sleep(1);

    boost::asio::ip::tcp::resolver resolver(client_ioc);
    boost::beast::tcp_stream stream(client_ioc);

    std::string host = "0.0.0.0";
    auto const results = resolver.resolve(host, "9000");
    stream.connect(results);

    // These objects perform our I/O
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{ioc};

    auto ep = boost::asio::connect(ws.next_layer(), results);

    host += ':' + std::to_string(ep.port());

    ws.set_option(boost::beast::websocket::stream_base::decorator(
            [](boost::beast::websocket::request_type &req) {
                req.set(boost::beast::http::field::user_agent, "Copper");
            }));

    ws.handshake(host, "/");

    // Send the message
    ws.write(boost::asio::buffer(std::string("hello")));

    boost::beast::flat_buffer buffer;

    ws.read(buffer);

    ws.close(boost::beast::websocket::close_code::normal);

    std::cout << boost::beast::make_printable(buffer.data()) << std::endl;

    try {
        ioc.stop();
        thread.join();
    } catch (std::exception &e) {

    }
}