#ifndef COPPER_COMPONENTS_CONTAINERS_HPP
#define COPPER_COMPONENTS_CONTAINERS_HPP

#pragma once

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
typedef std::unordered_map<std::string, std::string> unordered_map_of_strings;
typedef std::map<std::string, std::string> map_of_strings;

template <typename T, typename S>
using vector_of_shared_pairs = std::vector<std::pair<T, shared<S>>>;

template <typename T>
using vector_of = std::vector<T>;

template <typename T, typename S>
using map_of = std::map<T, S>;

template <typename T>
using queue_of = std::queue<T>;

template <typename T>
using optional_of = boost::optional<T>;

template <typename T>
using async_of = boost::asio::awaitable<
    T, boost::asio::strand<boost::asio::io_context::executor_type>>;
}  // namespace copper::components::containers

#endif