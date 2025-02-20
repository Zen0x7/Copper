#include <copper/components/http_router.hpp>

namespace copper::components {
http_route http_router::factory(http_method method, const char *path) {
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

shared<http_router> http_router::push(http_method method, const char *path,
                                      const shared<http_controller> &controller,
                                      http_controller_config config) {
  controller->set_config(config);

  get_routes()->push_back(
      std::pair(components::http_router::factory(method, path), controller));

  return shared_from_this();
}

shared<http_routes> http_router::get_routes() { return routes_; }

http_router::http_router() : routes_(boost::make_shared<http_routes>()) {}
}  // namespace copper::components