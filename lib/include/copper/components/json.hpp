#pragma once

#include <boost/json/value.hpp>
#include <boost/json/object.hpp>
#include <boost/json/array.hpp>
#include <boost/json/value_from.hpp>

namespace copper::components::json {
    typedef boost::json::value value;
    typedef boost::json::object object;
    typedef boost::json::array array;
    typedef boost::json::string string;
}