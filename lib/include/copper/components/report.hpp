#pragma once

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast


#include <boost/beast.hpp>
#include <iostream>
#include <boost/stacktrace.hpp>

#ifndef BOOST_STACKTRACE_USE_BACKTRACE
#define BOOST_STACKTRACE_USE_BACKTRACE
#endif

namespace copper::components {
    void report(
            boost::beast::error_code ec,
            char const *what
    );
}