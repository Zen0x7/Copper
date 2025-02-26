#ifndef COPPER_COMPONENTS_VIEW_HPP
#define COPPER_COMPONENTS_VIEW_HPP

#pragma once

#include <copper/components/shared.hpp>
#include <inja/inja.hpp>

namespace copper::components {

/**
 * View
 */
class view : public shared_enabled<view> {
 public:
  /**
   * Template
   */
  inja::Template template_;

  /**
   * Path
   */
  std::string path_;

  /**
   * Constructor
   *
   * @param path
   * @param environment
   */
  view(const std::string &path, const shared<inja::Environment> &environment) {
    path_ = path;
    template_ = environment->parse_template("views/" + path_ + ".html");
  }
};
}  // namespace copper::components

#endif