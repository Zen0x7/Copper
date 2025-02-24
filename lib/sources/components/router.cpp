#include <copper/components/router.hpp>

namespace copper::components {

http_route router::factory(http_method method, const char *path) {
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

shared<router> router::push(http_method method, const char *path,
                            const shared<http_controller> &controller,
                            http_controller_configuration config) {
  controller->set_configuration(config);

  get_routes()->push_back(
      std::pair(components::router::factory(method, path), controller));

  return shared_from_this();
}

shared<http_routes> router::get_routes() { return routes_; }

router::router() : routes_(boost::make_shared<http_routes>()) {}
}  // namespace copper::components