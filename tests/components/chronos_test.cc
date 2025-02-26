#include <gtest/gtest.h>

#include <copper/components/chronos.hpp>

using namespace copper::components;

TEST(Components_Chronos, To_Timestamp) {
  const auto _now = std::chrono::system_clock::now();
  const auto _timestamp = chronos::to_timestamp(_now);

  ASSERT_TRUE(_timestamp > 0);
}