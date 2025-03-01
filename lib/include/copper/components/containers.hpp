#ifndef COPPER_COMPONENTS_CONTAINERS_HPP
#define COPPER_COMPONENTS_CONTAINERS_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/optional.hpp>
#include <copper/components/shared.hpp>
#include <map>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

namespace copper::components::containers {

/**
 * Unordered maps of strings
 */
typedef std::unordered_map<std::string, std::string> unordered_map_of_strings;

/**
 * Map of strings
 */
typedef std::map<std::string, std::string> map_of_strings;

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
using async_of = boost::asio::awaitable<
    T, boost::asio::strand<boost::asio::io_context::executor_type>>;

}  // namespace copper::components::containers

#endif