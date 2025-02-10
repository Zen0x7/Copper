#pragma once

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast


#include <boost/beast.hpp>

#include <copper/components/mime_type.hpp>
#include <copper/components/normalized_path.hpp>

#ifndef HTTP_SERVER_HEADER_CONTENT
#define HTTP_SERVER_HEADER_CONTENT "Copper"
#endif

namespace copper::components::http_kernel {

    template<class Body, class Allocator>
    boost::beast::http::message_generator handle(
            boost::beast::string_view,
            boost::beast::http::request<
                    Body,
                    boost::beast::http::basic_fields<
                            Allocator
                    >
            > &&req
    ) {
        auto const not_found =
                [&req](boost::beast::string_view target) {
                    boost::beast::http::response<boost::beast::http::string_body> res{
                            boost::beast::http::status::not_found, req.version()};
                    res.set(boost::beast::http::field::server, HTTP_SERVER_HEADER_CONTENT);
                    res.set(boost::beast::http::field::content_type, "text/html");
                    res.keep_alive(req.keep_alive());
                    res.body() = "The resource '" + std::string(target) + "' was not found.";
                    res.prepare_payload();
                    return res; };

        return not_found(req.target());
    }

}