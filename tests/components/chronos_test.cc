#include <gtest/gtest.h>

#include <copper/components/chronos.hpp>

using namespace copper::components;

TEST(Components_Chronos, To_Timestamp) {
  auto now = std::chrono::system_clock::now();
  const auto timestamp = chronos::to_timestamp(now);

  ASSERT_TRUE(timestamp > 0);
}