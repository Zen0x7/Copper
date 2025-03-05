#ifndef COPPER_COMPONENTS_VIEWS_HPP
#define COPPER_COMPONENTS_VIEWS_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

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
  containers::map_of<std::string, shared<view>> items_;

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
    for (auto& [_name, _view] : items_) {
      if (_name == name) {
        return environment_->render(_view->template_, data);
      }
    }

    return environment_->render(items_.at("404")->template_, data);
  }

  /**
   * Get instance
   *
   * @return shared<views>
   */
  static shared<views> instance();

 private:
  /**
   * Instance
   */
  static shared<views> instance_;

  /**
   * Initialization flag
   */
  static std::once_flag initialization_flag_;
};

}  // namespace copper::components

#endif