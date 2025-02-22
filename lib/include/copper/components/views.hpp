#ifndef COPPER_COMPONENTS_VIEWS_HPP
#define COPPER_COMPONENTS_VIEWS_HPP

#pragma once

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
  views() : environment_(boost::make_shared<inja::Environment>()), items_() {

    // Default routes ...
    push("404", "404");
  }

  shared<views> push(std::string name, std::string path) {
    items_.insert(std::make_pair(
      name,
      boost::make_shared<view>(path, environment_)));

    return shared_from_this();
  }

  /**
   * Render
   *
   * @param name
   * @param data
   * @return string
   */
  std::string render(std::string name, json::json data = {}) {
    for (auto & item : items_) {
      if (item.first == name) {
        return environment_->render(item.second->template_, data);
      }
    }

    return environment_->render(items_.at("404")->template_, data);
  }
};

}  // namespace copper::components

#endif