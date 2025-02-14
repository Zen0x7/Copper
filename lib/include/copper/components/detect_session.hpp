#pragma once


#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/scope/scope_exit.hpp>

#include <copper/components/http_session.hpp>

namespace copper::components {

    /**
     * Detect session protocol
     *
     * @param stream
     * @param ctx
     * @param doc_root
     * @return boost::asio::awaitable<T, U> Callback
     */
    boost::asio::awaitable<
            void,
            boost::asio::strand<
                    boost::asio::io_context::executor_type
            >
    > detect_session(
            shared<state> & state,
            typename boost::beast::tcp_stream::rebind_executor<
                    boost::asio::strand<
                            boost::asio::io_context::executor_type
                    >
            >::other stream,
            boost::asio::ssl::context &ctx,
            boost::beast::string_view doc_root
    );

} // namespace copper::component