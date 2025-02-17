#pragma once

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast

#include <boost/beast/core/string.hpp>

namespace copper::components {

    /**
     * Get MIME type of path
     *
     * @param path
     * @return std::string Output
     */
    boost::beast::string_view mime_type(
            boost::beast::string_view path
    );

} // namespace copper::component