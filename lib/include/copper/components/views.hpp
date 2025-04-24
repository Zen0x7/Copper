#ifndef COPPER_COMPONENTS_VIEWS_HPP
#define COPPER_COMPONENTS_VIEWS_HPP

#pragma once

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

#include <boost/filesystem.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/json.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/view.hpp>
#include <inja/inja.hpp>

namespace copper::components {

/**
 * Views
 */
class views : public shared_enabled<views> {
  /**
   * Environment
   */
  shared<inja::Environment> environment_;

  /**
   * Items
   */
  map_of<std::string, shared<view>> items_;

 public:
  /**
   * Constructor
   */
  views() : environment_(boost::make_shared<inja::Environment>()) {}

  shared<views> push(std::string name, std::string path) {
    items_.insert(
        std::make_pair(name, boost::make_shared<view>(path, environment_)));

    return shared_from_this();
  }

  /**
   * Render
   *
   * @param name
   * @param data
   * @return string
   */
  std::string render(std::string name, json::json data = {}) const {
    // LCOV_EXCL_START
    for (auto& [_name, _view] : items_) {
      if (_name == name && _name != "404") {
        return environment_->render(_view->template_, data);
      }
    }
    // LCOV_EXCL_STOP

    return environment_->render(items_.at("404")->template_, data);
  }

  /**
   * Factory
   *
   * @return shared<views>
   */
  static shared<views> factory() { return boost::make_shared<views>(); }
};

}  // namespace copper::components

#endif
