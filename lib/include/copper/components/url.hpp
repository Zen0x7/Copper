#ifndef COPPER_COMPONENTS_URL_HPP
#define COPPER_COMPONENTS_URL_HPP

#pragma once

#include <copper/components/request.hpp>

namespace copper::components {
    std::string url_from_request(const request & request);
}

#endif