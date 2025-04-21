#ifndef COPPER_COMPONENTS_JSON_HPP
#define COPPER_COMPONENTS_JSON_HPP

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

#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
#include <boost/json/serialize.hpp>
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
