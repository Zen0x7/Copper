//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include <copper/components/normalized_path.hpp>

TEST(Components_Filesystem_NormalizedPath, Usage) {
  using namespace copper::components;

  const auto _value = normalized_path("srv/app/bin/", "/../main.cpp");

  ASSERT_EQ(_value, "srv/app/bin/../main.cpp");
}