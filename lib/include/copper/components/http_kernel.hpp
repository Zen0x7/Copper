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
            boost::beast::string_view doc_root,
            boost::beast::http::request<
                    Body,
                    boost::beast::http::basic_fields<
                            Allocator
                    >
            > &&req
    ) {
        auto const bad_request =
                [&req](boost::beast::string_view why) {
                    boost::beast::http::response<boost::beast::http::string_body> res{
                            boost::beast::http::status::bad_request, req.version()};
                    res.set(boost::beast::http::field::server, HTTP_SERVER_HEADER_CONTENT);
                    res.set(boost::beast::http::field::content_type, "text/html");
                    res.keep_alive(req.keep_alive());
                    res.body() = std::string(why);
                    res.prepare_payload();
                    return res;
                };

        auto const not_found =
                [&req](boost::beast::string_view target) {
                    boost::beast::http::response<boost::beast::http::string_body> res{
                            boost::beast::http::status::not_found, req.version()};
                    res.set(boost::beast::http::field::server, HTTP_SERVER_HEADER_CONTENT);
                    res.set(boost::beast::http::field::content_type, "text/html");
                    res.keep_alive(req.keep_alive());
                    res.body() = "The resource '" + std::string(target) + "' was not found.";
                    res.prepare_payload();
                    return res;
                };

        auto const server_error =
                [&req](boost::beast::string_view what) {
                    boost::beast::http::response<boost::beast::http::string_body> res{
                            boost::beast::http::status::internal_server_error, req.version()};
                    res.set(boost::beast::http::field::server, HTTP_SERVER_HEADER_CONTENT);
                    res.set(boost::beast::http::field::content_type, "text/html");
                    res.keep_alive(req.keep_alive());
                    res.body() = "An error occurred: '" + std::string(what) + "'";
                    res.prepare_payload();
                    return res;
                };

        if (req.method() != boost::beast::http::verb::get &&
            req.method() != boost::beast::http::verb::head)
            return bad_request("Unknown HTTP-method");

        if (req.target().empty() ||
            req.target()[0] != '/' ||
            req.target().find("..") != boost::beast::string_view::npos)
            return bad_request("Illegal request-target");

        std::string path = normalized_path(doc_root, req.target());
        if (req.target().back() == '/')
            path.append("index.html");

        boost::beast::error_code ec;
        boost::beast::http::file_body::value_type body;
        body.open(path.c_str(), boost::beast::file_mode::scan, ec);

        if (ec == boost::beast::errc::no_such_file_or_directory)
            return not_found(req.target());

        if (ec)
            return server_error(ec.message());

        auto const size = body.size();

        if (req.method() == boost::beast::http::verb::head) {
            boost::beast::http::response<boost::beast::http::empty_body> res{boost::beast::http::status::ok,
                                                                             req.version()};
            res.set(boost::beast::http::field::server, HTTP_SERVER_HEADER_CONTENT);
            res.set(boost::beast::http::field::content_type, mime_type(path));
            res.content_length(size);
            res.keep_alive(req.keep_alive());
            return res;
        }

        boost::beast::http::response<boost::beast::http::file_body> res{
                std::piecewise_construct,
                std::make_tuple(std::move(body)),
                std::make_tuple(boost::beast::http::status::ok, req.version())};
        res.set(boost::beast::http::field::server, HTTP_SERVER_HEADER_CONTENT);
        res.set(boost::beast::http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return res;
    }

}