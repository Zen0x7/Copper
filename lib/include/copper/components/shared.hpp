#ifndef COPPER_COMPONENTS_SHARED_HPP
#define COPPER_COMPONENTS_SHARED_HPP

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

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr.hpp>

namespace copper::components {

/**
 * Shared
 */
template <typename T>
using shared = boost::shared_ptr<T>;

/**
 * Weaker
 */
template <typename T>
using weaker = boost::weak_ptr<T>;

/**
 * Basic shared
 */
template <typename T>
using basic_shared = std::shared_ptr<T>;

/**
 * Shared enabled
 */
template <typename T>
using shared_enabled = boost::enable_shared_from_this<T>;

/**
 * Basic shared enabled
 */
template <typename T>
using basic_shared_enabled = std::enable_shared_from_this<T>;
}  // namespace copper::components

#endif
