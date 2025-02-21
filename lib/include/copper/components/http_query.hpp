#ifndef COPPER_COMPONENTS_HTTP_QUERY_HPP
#define COPPER_COMPONENTS_HTTP_QUERY_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/http_request.hpp>
#include <copper/components/json.hpp>
#include <istream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace copper::components {

std::string http_query_from_request(const http_request &request);

}  // namespace copper::components

#endif