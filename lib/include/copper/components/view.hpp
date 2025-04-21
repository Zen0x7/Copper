#ifndef COPPER_COMPONENTS_VIEW_HPP
#define COPPER_COMPONENTS_VIEW_HPP

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
#include <copper/components/configuration.hpp>
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
  view(const std::string& path, const shared<inja::Environment>& environment)
      : path_(path) {
    template_ = environment->parse_template(
        dotenv::getenv("WORKING_DIRECTORY", "/__w/Copper/Copper/bin") +
        "/views/" + path_ + ".html");
  }
};
}  // namespace copper::components

#endif
