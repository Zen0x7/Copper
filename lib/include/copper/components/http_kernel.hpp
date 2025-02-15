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
#include <copper/components/shared.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/normalized_path.hpp>

#include <copper/components/http_route.hpp>
#include <copper/components/http_route_match.hpp>
#include <copper/components/http_route_find.hpp>
#include <copper/components/http_controller.hpp>

#include <copper/components/http_response_generic.hpp>
#include <copper/components/http_response_too_many_requests.hpp>
#include <copper/components/http_response_unauthorized.hpp>
#include <copper/components/http_response_exception.hpp>
#include <copper/components/http_response_cors.hpp>
#include <copper/components/http_response_bad_request.hpp>
#include <copper/components/http_response_not_found.hpp>

#include <copper/components/state.hpp>

#include <boost/algorithm/string.hpp>

#include <copper/components/authentication.hpp>
#include <copper/components/validator.hpp>

#ifndef HTTP_SERVER_HEADER_CONTENT
#define HTTP_SERVER_HEADER_CONTENT "Copper"
#endif

namespace copper::components {
    struct http_kernel_result {
        http_route route_;
        shared<http_controller> controller_;
        containers::unordered_map_of_strings bindings_;
    };

    class http_kernel : public shared_enabled<http_kernel> {
        shared<state> state_;
    public:
        explicit http_kernel(const shared<state> &state) : state_(state) {}

        containers::optional_of<http_kernel_result> find_on_routes(
                const http_request & request
                ) const {
            for (const auto & [route, controller] : *state_->get_router()->get_routes()) {
                if (auto [matches, bindings] = http_route_match(request.method(), request.target(), route); matches) {
                    return http_kernel_result {
                        .route_ = route,
                        .controller_ = controller,
                        .bindings_ = bindings
                    };
                }
            }

            return boost::none;
        }

        containers::vector_of<http_method> get_available_methods(const http_request & request) const {
            containers::vector_of<http_method> methods;
            for (const auto& [route, controller] : *state_->get_router()->get_routes()) {
                if (auto [matches, bindings] = http_route_find(request.target(), route); matches)
                    methods.push_back(route.method_);
            }
            return methods; }

        boost::asio::awaitable<
                http_response_generic,
                boost::asio::strand<
                        boost::asio::io_context::executor_type
                >
        > invoke(
            boost::beast::string_view /* root */,
            const http_request & request,
            const std::string & ip
        ) const {
            const auto now = chronos::now();

            if (const auto route = find_on_routes(request); route.has_value()) {
                containers::unordered_map_of_strings bindings = route.value().bindings_;

                if (route.value().controller_->requires_limitation()) {
                    if (auto [limited, TTL] = co_await state_->get_redis()->is_alive(request, ip, route.value().controller_->requests_per_minute()); limited) {
                        co_return http_response_too_many_requests(request, now, TTL);
                    }
                }

                if (route.value().controller_->requires_authentication()) {
                    std::string bearer {request["Authorization"]};

                    std::string token = boost::starts_with(bearer, "Bearer ") ? bearer.substr(7) : bearer;

                    boost::optional<authentication_result> user_id;
                    user_id = authentication_from_bearer(bearer, dotenv::getenv("APP_KEY"));

                    if (!user_id.has_value()) co_return http_response_unauthorized(request, now);

                    route.value().controller_->set_user(user_id.get().id);
                }

                route.value().controller_->set_bindings(bindings);
                route.value().controller_->set_state(state_);

                if (route.value().controller_->requires_data()) {
                    boost::system::error_code json_parse_error_code;
                    boost::json::value value = boost::json::parse(request.body(), json_parse_error_code);

                    if (!json_parse_error_code) {
                        route.value().controller_->set_data(value);
                        auto rules = route.value().controller_->validate_data();

                        if (auto validator = validator_make(rules, value); !validator->success) {
                            auto error_response = boost::json::object(
                                    {{"message", "The given data was invalid."}, {"errors", validator->errors}});

                            co_return route.value().controller_->response(
                                    request, boost::beast::http::status::unprocessable_entity,
                                    serialize(error_response), "application/json", now);
                        }
                    } else {
                        auto error_response
                                = boost::json::object({{"message", "The given data was invalid."},
                                                       {"errors", {{"*", "The body must be a valid JSON."}}}});

                        co_return route.value().controller_->response(
                                request, boost::beast::http::status::unprocessable_entity, serialize(error_response),
                                "application/json", now);
                    }
                }

                try {
                    co_return route.value().controller_->invoke(request);
                } catch (std::exception& exception) {
                    std::cout << exception.what() << std::endl;
                    co_return http_response_exception(request, now);
                }
            }

            if (request.method() == http_method::options) {
                auto available_verbs = get_available_methods(request);
                co_return http_response_cors(request, now, available_verbs);
            }

            if (http_request_is_illegal(request)) {
                co_return http_response_bad_request(request, now);
            }

            co_return http_response_not_found(request, now);
        }
    };


    /**
     * Kernel
     *
     * @tparam Body
     * @tparam Allocator
     * @param req
     * @return
     */
    template<
            class Body,
            class Allocator
    >
    boost::beast::http::message_generator http_kernel_basic_handler(
            shared<state> & state,
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
                    res.set(boost::beast::http::field::server, "Copper");
                    res.set(boost::beast::http::field::content_type, "text/html");
                    res.keep_alive(req.keep_alive());
                    res.body() = "The resource '" + std::string(target) + "' was not found.";
                    res.prepare_payload();
                    return res; };

        return not_found(req.target());
    }

} // namespace copper::component