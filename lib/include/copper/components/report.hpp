#pragma once

#include <boost/beast.hpp>
#include <iostream>
#include <boost/stacktrace.hpp>

#ifndef BOOST_STACKTRACE_USE_BACKTRACE
#define BOOST_STACKTRACE_USE_BACKTRACE
#endif

namespace copper::components {
    void report(boost::beast::error_code ec, char const* what);
}