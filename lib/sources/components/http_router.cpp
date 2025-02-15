#include <copper/components/http_router.hpp>

namespace copper::components {
    http_route http_router::factory(http_method method, const char* path) {
        std::string route(path);

        const auto expression = expression_make(route);

        return {
            .method_ = method,
            .url_ = route,
            .signature_ = path,
            .is_expression_ = !expression->get_arguments().empty(),
            .expression_ = expression,
        };
    }
}