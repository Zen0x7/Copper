#ifndef COPPER_COMPONENTS_JSON_HPP
#define COPPER_COMPONENTS_JSON_HPP

#pragma once

#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/value.hpp>
#include <boost/json/value_from.hpp>

namespace copper::components::json {

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