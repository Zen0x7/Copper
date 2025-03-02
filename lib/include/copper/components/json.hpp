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
using json = nlohmann::json;

/**
 * JSON value
 */
using value = boost::json::value;

/**
 * JSON object
 */
using object = boost::json::object;

/**
 * JSON array
 */
using array = boost::json::array;

/**
 * JSON string
 */
using string = boost::json::string;

}  // namespace copper::components::json

#endif