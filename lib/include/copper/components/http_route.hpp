#pragma once

#include <boost/beast.hpp>

#include <copper/components/expression.hpp>

#include <copper/components/containers.hpp>
#include <copper/components/shared.hpp>

#include <copper/components/http_method.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace copper::components {
    struct http_route_result {
        bool matches_;
        containers::unordered_map_of_strings bindings_;
    };

    struct http_route {
        http_method method_;
        std::string url_;
        std::string signature_;
        bool is_expression_ = false;
        shared<expression> expression_;
    };
}  // namespace copper::components
