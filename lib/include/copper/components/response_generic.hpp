#ifndef COPPER_COMPONENTS_RESPONSE_GENERIC_HPP
#define COPPER_COMPONENTS_RESPONSE_GENERIC_HPP

#pragma once

#include <boost/beast/http/message_generator.hpp>

namespace copper::components {

/**
 * Response generic type
 */
typedef boost::beast::http::message_generator response_generic;

}  // namespace copper::components

#endif