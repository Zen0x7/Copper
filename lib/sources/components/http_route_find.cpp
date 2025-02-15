#include <copper/components/http_route_find.hpp>

namespace copper::components {

    http_route_result http_route_find(const std::string_view &path, const http_route &route) {
        const size_t query_ask_symbol_position = path.find('?');
        const bool path_has_parameters = query_ask_symbol_position != std::string::npos;
        const std::string to_be_compared {
                path_has_parameters ? path.substr(0, query_ask_symbol_position) : path
        };

        if (route.is_expression_) {
            const std::string query{path};
            const auto expression_result = route.expression_->query(query);
            return {
                    .matches_ = expression_result->matches(),
                    .bindings_ = expression_result->get_bindings(),
            };
        }

        if (route.signature_ == to_be_compared) return { .matches_ = true, .bindings_ = {} };

        return {
                .matches_ = false,
                .bindings_ = {},
        };
    }
}