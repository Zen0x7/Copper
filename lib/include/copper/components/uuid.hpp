#ifndef COPPER_COMPONENTS_UUID_HPP
#define COPPER_COMPONENTS_UUID_HPP

#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace copper::components {
using uuid = boost::uuids::uuid;
}

#endif