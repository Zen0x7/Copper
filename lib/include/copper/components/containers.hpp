#ifndef COPPER_COMPONENTS_CONTAINERS_HPP
#define COPPER_COMPONENTS_CONTAINERS_HPP

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

#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/optional.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>
#include <map>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

namespace copper::components::containers {

/**
 * Unordered map transparent hasher
 */
struct unordered_map_transparent_hasher {
  using is_transparent = void;

  size_t operator()(std::string_view sv) const {
    return std::hash<std::string_view>{}(sv);
  }
};

/**
 * Unordered map of strings
 */
using unordered_map_of_strings =
    std::unordered_map<std::string,
                       std::string,
                       unordered_map_transparent_hasher,
                       std::equal_to<>>;

/**
 * Unordered map of identified T
 */
template <typename T>
using uuid_hash_map_of = std::unordered_map<uuid, T, boost::hash<uuid>>;

/**
 * Map of strings
 */
using map_of_strings = std::map<std::string, std::string, std::less<>>;

/**
 * Vector of shared pairs
 */
template <typename T, typename S>
using vector_of_shared_pairs = std::vector<std::pair<T, shared<S>>>;

/**
 * Vector of
 */
template <typename T>
using vector_of = std::vector<T>;

/**
 * Map of
 */
template <typename T, typename S>
using map_of = std::map<T, S>;

/**
 * Queue of
 */
template <typename T>
using queue_of = std::queue<T>;

/**
 * Tuple of
 */
template <typename... T>
using tuple_of = std::tuple<T...>;

/**
 * Optional of
 */
template <typename T>
using optional_of = boost::optional<T>;

/**
 * Async of
 */
template <typename T>
using async_of = boost::asio::awaitable<T>;

/**
 * Callback of
 */
template <typename... T>
using callback_of = std::function<void(T...)>;

/**
 * Callback
 */
using callback = std::function<void()>;

}  // namespace copper::components::containers

#endif
