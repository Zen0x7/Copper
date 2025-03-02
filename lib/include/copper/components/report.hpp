#ifndef COPPER_COMPONENTS_REPORT_HPP
#define COPPER_COMPONENTS_REPORT_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/beast/core/error.hpp>

namespace copper::components {

/**
 * Report
 *
 * @param ec
 * @param what
 */
void report(const boost::beast::error_code &ec, char const *what);

/**
 * Report for OpenSSL
 */
void report_for_openssl();

}  // namespace copper::components

#endif