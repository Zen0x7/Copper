#ifndef COPPER_COMPONENTS_SHARED_HANDLER_HPP
#define COPPER_COMPONENTS_SHARED_HANDLER_HPP

#pragma once

#include <boost/algorithm/string/predicate.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/gunzip.hpp>
#include <copper/components/request.hpp>
#include <copper/components/response.hpp>
#include <copper/components/views.hpp>

namespace copper::components {
response response_shared_handler(const request &request, response &response,
                                 long start_at, int ttl = -1);
}

#endif