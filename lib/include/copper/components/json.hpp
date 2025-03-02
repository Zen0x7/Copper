#ifndef COPPER_COMPONENTS_JSON_HPP
#define COPPER_COMPONENTS_JSON_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
#include <boost/json/value_from.hpp>
#include <nlohmann/json.hpp>

namespace copper::components::json {

/**
 * JSON
 */
using nlohmann::json;

/**
 * JSON value
 */
typedef boost::json::value value;

/**
 * JSON object
 */
typedef boost::json::object object;

/**
 * JSON array
 */
typedef boost::json::array array;

/**
 * JSON string
 */
typedef boost::json::string string;

}  // namespace copper::components::json

#endif