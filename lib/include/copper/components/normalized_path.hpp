#ifndef COPPER_COMPONENTS_NORMALIZED_PATH_HPP
#define COPPER_COMPONENTS_NORMALIZED_PATH_HPP

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

#include <boost/beast/core/string.hpp>

namespace copper::components {

/**
 * Get normalized path
 *
 * @param base
 * @param path
 * @return string Output
 */
inline std::string normalized_path(boost::beast::string_view base,
                                   boost::beast::string_view path) {
  if (base.empty())
    return std::string(path);

  std::string result(base);

#ifdef BOOST_MSVC
  char constexpr separator = '\\';
  if (result.back() == separator)
    result.resize(result.size() - 1);
  if (!path.empty() && path.front() != separator)
    result += separator;
  result.append(path.data(), path.size());
  for (auto& c : result)
    if (c == '/')
      c = separator;
#else
  char constexpr separator = '/';
  if (result.back() == separator)
    result.resize(result.size() - 1);
  if (!path.empty() && path.front() != separator)
    result += separator;
  result.append(path.data(), path.size());
#endif

  return result;
}

}  // namespace copper::components

#endif
