#ifndef COPPER_COMPONENTS_REPORT_HPP
#define COPPER_COMPONENTS_REPORT_HPP

#pragma once

#include <boost/beast/core/error.hpp>
#include <boost/stacktrace.hpp>

#ifndef BOOST_STACKTRACE_USE_BACKTRACE
#define BOOST_STACKTRACE_USE_BACKTRACE
#endif

namespace copper::components {

/**
 * Report
 *
 * @param ec
 * @param what
 */
void report(boost::beast::error_code ec, char const *what);

/**
 * Report for OpenSSL
 */
void report_for_openssl();

}  // namespace copper::components

#endif