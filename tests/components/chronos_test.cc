//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include <copper/components/chronos.hpp>

using namespace copper::components;

TEST(Components_Chronos, To_Timestamp) {
  const auto _now = std::chrono::system_clock::now();
  const auto _timestamp = chronos::to_timestamp(_now);

  ASSERT_TRUE(_timestamp > 0);
}