#ifndef COPPER_COMPONENTS_HTTP_RESPONSE_GENERIC_HPP
#define COPPER_COMPONENTS_HTTP_RESPONSE_GENERIC_HPP

#pragma once

#include <boost/beast/http/message_generator.hpp>

namespace copper::components {

/**
 * HTTP response generic type
 */
typedef boost::beast::http::message_generator http_response_generic;

}  // namespace copper::components

#endif