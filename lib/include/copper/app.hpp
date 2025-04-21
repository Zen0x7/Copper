#ifndef COPPER_APP_HPP
#define COPPER_APP_HPP

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

#include <copper/components/configuration.hpp>
#include <string>

namespace copper {

/**
 * Get version
 *
 * @return string
 */
std::string get_version();

/**
 * Run
 *
 * @param argc
 * @param argv
 * @return int
 */
int run(int argc, const char* argv[]);

}  // namespace copper

#endif  // COPPER_APP_HPP
