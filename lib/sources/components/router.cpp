// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <copper/components/router.hpp>

namespace copper::components {

route router::make_route(const method method, const char* path) {
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

shared<router> router::factory() {
  return boost::make_shared<router>();
}

shared<router> router::push(
    const method method,
    const char* path,
    const shared<controller>& controller,
    const controller_configuration controller_configuration) {
  controller->set_controller_configuration(controller_configuration);

  if (auto _route = make_route(method, path); _route.is_expression_) {
    get_routes()->push_back(std::pair(std::move(_route), controller));
  } else {
    get_routes()->insert(get_routes()->begin(),
                         std::pair(std::move(_route), controller));
  }

  return shared_from_this();
}

router::router() = default;
}  // namespace copper::components
