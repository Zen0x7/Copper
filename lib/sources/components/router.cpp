#include <copper/components/router.hpp>

namespace copper::components {

route router::factory(const method method, const char *path) {
  const std::string route(path);

  const auto expression = expression_make(route);

  return {
      .method_ = method,
      .url_ = route,
      .signature_ = path,
      .is_expression_ = !expression->get_arguments().empty(),
      .expression_ = expression,
  };
}

shared<router> router::push(const method method, const char *path,
                            const shared<controller> &controller,
                            const controller_configuration config) {
  controller->set_configuration(config);

  get_routes()->push_back(std::pair(factory(method, path), controller));

  return shared_from_this();
}

shared<routes> router::get_routes() { return routes_; }

router::router() : routes_(boost::make_shared<routes>()) {}
}  // namespace copper::components