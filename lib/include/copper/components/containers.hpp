#pragma once

#include <copper/components/shared.hpp>

#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <queue>

#include <boost/optional.hpp>

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
}