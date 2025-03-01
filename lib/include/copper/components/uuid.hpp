#ifndef COPPER_COMPONENTS_UUID_HPP
#define COPPER_COMPONENTS_UUID_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace copper::components {

/**
 * UUID
 */
using uuid = boost::uuids::uuid;

}  // namespace copper::components

#endif