#pragma once

#include <boost/beast.hpp>

#include <copper/components/report.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/scope/scope_exit.hpp>

namespace copper::components {

    template<typename Stream>
    boost::asio::awaitable<void, boost::asio::strand<boost::asio::io_context::executor_type>>
    websocket_session_run(
            Stream& stream,
            boost::beast::flat_buffer& buffer,
            boost::beast::http::request<boost::beast::http::string_body> req,
            boost::beast::string_view)
    {
        auto cs = co_await boost::asio::this_coro::cancellation_state;
        auto ws = boost::beast::websocket::stream<Stream&>{ stream };

        ws.set_option(
                boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));

        ws.set_option(boost::beast::websocket::stream_base::decorator(
                [](boost::beast::websocket::response_type& res)
                {
                    res.set(boost::beast::http::field::server, "Copper");
                }));

        co_await ws.async_accept(req);

        while(!cs.cancelled())
        {
            auto [ec, _] = co_await ws.async_read(buffer, boost::asio::as_tuple);

            if(ec == boost::beast::websocket::error::closed || ec == boost::asio::ssl::error::stream_truncated)
                co_return;

            if(ec)
                throw boost::system::system_error{ ec };

            ws.text(ws.got_text());
            co_await ws.async_write(buffer.data());

            buffer.consume(buffer.size());
        }

        auto [ec] = co_await ws.async_close(
                boost::beast::websocket::close_code::service_restart, boost::asio::as_tuple);

        if(ec && ec != boost::asio::ssl::error::stream_truncated)
            throw boost::system::system_error{ ec };
    }
}