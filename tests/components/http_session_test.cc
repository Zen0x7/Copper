#include <gtest/gtest.h>

#include <copper/components/certificates.hpp>
#include <copper/components/task_group.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/state.hpp>
#include <copper/components/signal_handler.hpp>
#include <copper/components/http_method.hpp>
#include <copper/components/http_response.hpp>
#include <copper/components/http_fields.hpp>
#include <copper/components/http_controller.hpp>
#include <copper/components/http_status_code.hpp>
#include <copper/components/json.hpp>
#include <copper/components/dotenv.hpp>
#include <copper/components/chronos.hpp>

#include <boost/beast.hpp>

using namespace copper::components;

class heartbeat_controller final : public http_controller {
public:
    bool requires_limitation() const override { return true; }
    int requests_per_minute() const override { return 5; }
    http_response invoke(const http_request & request) override {
        auto now = chronos::now();
        const json::object data = {
                { "message", "Request has been processed." },
                {"data", "pong"},
                { "timestamp", now },
                {"status", 200}
        };
        return response(request, http_status_code::ok, serialize(data), "application/json", now);
    }
};

class params_controller final : public http_controller {
public:
    bool requires_limitation() const override { return true; }
    int requests_per_minute() const override { return 5; }
    http_response invoke(const http_request & request) override {
        auto now = chronos::now();
        const json::object data = {
                { "message", "Request has been processed." },
                {"data", this->bindings_.at("name") },
                { "timestamp", now },
                {"status", 200}
        };
        return response(request, http_status_code::ok, serialize(data), "application/json", now);
    }
};

TEST(Components_TCP_Listener, Client) {

    dotenv::init();

    auto const address = boost::asio::ip::make_address("0.0.0.0");
    auto const port = 9001;
    auto const endpoint = boost::asio::ip::tcp::endpoint{address, port};
    auto const doc_root = std::string_view{"."};

    boost::asio::io_context ioc;

    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};

    load_server_certificate(ctx);

    task_group task_group{ioc.get_executor()};

    auto state_ = boost::make_shared<state>();

    state_->get_router()->get_routes()->push_back(
            std::pair(http_router::factory(http_method::get, "/api/up"), boost::make_shared<heartbeat_controller>())
    );

    state_->get_router()->get_routes()->push_back(
            std::pair(http_router::factory(http_method::get, "/api/params/{name}"), boost::make_shared<params_controller>())
    );

    boost::asio::co_spawn(
            boost::asio::make_strand(ioc),
            listener(state_, task_group, ctx, endpoint, doc_root),
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

    try {
        std::thread thread([&]() {
            try {
                ioc.run();
            } catch (std::exception &e) {

            }
        });

        thread.detach();

        sleep(1);

        boost::asio::ip::tcp::resolver resolver(client_ioc);
        boost::beast::tcp_stream stream(client_ioc);

        auto const host = "127.0.0.1";
        auto const results = resolver.resolve(host, "9001");
        stream.connect(results);

        http_request req{http_method::get, "/", 11};
        req.set(http_fields::host, host);
        req.set(http_fields::user_agent, "Copper");

        http_request options_up_request{http_method::options, "/api/up", 11};
        req.set(http_fields::host, host);
        req.set(http_fields::user_agent, "Copper");

        http_request up_request{http_method::get, "/api/up", 11};
        req.set(http_fields::host, host);
        req.set(http_fields::user_agent, "Copper");

        http_request params_request{http_method::get, "/api/params/{name}", 11};
        req.set(http_fields::host, host);
        req.set(http_fields::user_agent, "Copper");

        http_request close_req{http_method::get, "/", 11};
        close_req.set(http_fields::host, host);
        close_req.set(http_fields::user_agent, "Copper");
        close_req.set(http_fields::connection, "close");

        boost::beast::flat_buffer buffer;
        boost::beast::http::response<boost::beast::http::dynamic_body> res;

        boost::beast::http::write(stream, req);
        boost::beast::http::read(stream, buffer, res);

        boost::beast::http::write(stream, options_up_request);
        boost::beast::http::read(stream, buffer, res);

        for (int i = 0; i <= 5; i++) {
            boost::beast::http::write(stream, up_request);
            boost::beast::http::read(stream, buffer, res);
        }

        boost::beast::http::write(stream, params_request);
        boost::beast::http::read(stream, buffer, res);

        boost::beast::http::write(stream, close_req);
        boost::beast::http::read(stream, buffer, res);

        task_group.emit(boost::asio::cancellation_type::all);
        sleep(10);
        ioc.stop();
        thread.join();
    } catch (std::exception const &e) {

    }

    ASSERT_TRUE(true);
}