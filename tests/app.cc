//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include <copper/app.hpp>

using namespace copper;

TEST(App, Version) { ASSERT_EQ(get_version(), "7.0.0"); }
