//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/router.hpp>

namespace copper::components {

route router::factory(const method method, const char *path) {
  const std::string _route(path);

  const auto _expression = expression_make(_route);

  return {
      .method_ = method,
      .url_ = _route,
      .signature_ = path,
      .is_expression_ = !_expression->get_arguments().empty(),
      .expression_ = _expression,
  };
}

shared<router> router::instance_ = nullptr;

std::once_flag router::initialization_flag_;

shared<router> router::instance() {
  std::call_once(initialization_flag_,
                 [] { instance_ = boost::make_shared<router>(); });

  return instance_->shared_from_this();
}

shared<router> router::push(const method method, const char *path,
                            const shared<controller> &controller,
                            const controller_configuration config) {
  controller->set_configuration(config);

  if (auto _route = factory(method, path); _route.is_expression_) {
    get_routes()->push_back(std::pair(std::move(_route), controller));
  } else {
    get_routes()->insert(get_routes()->begin(),
                         std::pair(std::move(_route), controller));
  }

  return shared_from_this();
}

shared<routes> router::get_routes() { return routes_; }

router::router() : routes_(boost::make_shared<routes>()) {}
}  // namespace copper::components