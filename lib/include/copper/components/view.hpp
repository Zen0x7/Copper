#ifndef COPPER_COMPONENTS_VIEW_HPP
#define COPPER_COMPONENTS_VIEW_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

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