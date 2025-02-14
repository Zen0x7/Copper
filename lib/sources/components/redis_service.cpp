#include <copper/components/redis_service.hpp>

namespace copper::components {
    boost::asio::awaitable<int, boost::asio::strand<boost::asio::io_context::executor_type>>
    redis_service::exists(const std::string &key) {
        boost::redis::request request;
        boost::redis::response<int> response;
        request.push("EXISTS", key);
        auto connection = co_await this->get_redis_connection();
        co_await connection->async_exec(request, response, boost::asio::deferred);
        co_return std::get<0>(response).value();
    }

    boost::asio::awaitable<int64_t, boost::asio::strand<boost::asio::io_context::executor_type>>
    redis_service::counter_of(const std::string &key) {
        boost::redis::request request;
        boost::redis::response<int> response;
        request.push("GET", key);
        auto connection = co_await this->get_redis_connection();
        co_await connection->async_exec(request, response, boost::asio::deferred);
        co_return std::get<0>(response).value();
    }

    boost::asio::awaitable<int64_t, boost::asio::strand<boost::asio::io_context::executor_type>>
    redis_service::ttl_of(const std::string &key) {
        boost::redis::request request;
        boost::redis::response<int> response;
        request.push("TTL", key);
        auto connection = co_await this->get_redis_connection();
        co_await connection->async_exec(request, response, boost::asio::deferred);
        co_return std::get<0>(response).value();
    }

    boost::asio::awaitable<void, boost::asio::strand<boost::asio::io_context::executor_type>>
    redis_service::increase(const std::string &key) {
        boost::redis::request request;
        boost::redis::response<int> response;
        request.push("INCR", key);
        auto connection = co_await this->get_redis_connection();
        co_await connection->async_exec(request, response, boost::asio::deferred);
    }

    boost::asio::awaitable<void, boost::asio::strand<boost::asio::io_context::executor_type>>
    redis_service::set(const std::string &key) {
        boost::redis::request request;
        boost::redis::response<int> response;
        request.push("SET", key, 1, "EX", 60);
        auto connection = co_await this->get_redis_connection();
        co_await connection->async_exec(request, response, boost::asio::deferred);
    }

    std::string redis_service::get_key_of(const http_request &request, const std::string &ip) {
        const size_t query_ask_mark_position = request.target().find('?');
        const bool path_has_params = query_ask_mark_position != std::string::npos;
        const std::string path{path_has_params ? request.target().substr(0, query_ask_mark_position) : request.target()};
        std::string key = request.method_string();
        key.append(":");
        key.append(path);
        key.append(":");
        key.append(ip);
        return key;
    }

    boost::asio::awaitable<
            std::tuple<bool, int>,
            boost::asio::strand<
                    boost::asio::io_context::executor_type
            >
    > redis_service::is_alive(const http_request &request, const std::string &ip, const int &max_requests) {
        const auto key = get_key_of(request, ip);
        if (co_await exists(key) != 0) {
            const auto requests = co_await counter_of(key);
            co_await increase(key);
            if (requests >= max_requests) co_return std::tuple {true, co_await ttl_of(key)};
            co_return std::tuple {false, 0};
        }
        co_await set(key);
        co_return std::tuple {false, 0};
    }
}