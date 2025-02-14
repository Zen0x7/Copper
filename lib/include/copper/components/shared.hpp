#pragma once

#include <boost/smart_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace copper::components {
    template <typename T>
    using shared = boost::shared_ptr<T>;

    template <typename T>
    using shared_enabled = boost::enable_shared_from_this<T>;
}