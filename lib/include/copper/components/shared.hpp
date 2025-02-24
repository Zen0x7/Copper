#ifndef COPPER_COMPONENTS_SHARED_HPP
#define COPPER_COMPONENTS_SHARED_HPP

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr.hpp>

namespace copper::components {

/**
 * Shared
 */
template <typename T>
using shared = boost::shared_ptr<T>;

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