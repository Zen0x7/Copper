// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <gtest/gtest.h>
#include <copper/components/chronos.hpp>

using namespace copper::components;

class ChronosTestFixture : public ::testing::Test {
 protected:
  std::chrono::system_clock::time_point now;

  void SetUp() override { now = std::chrono::system_clock::now(); }
};

TEST_F(ChronosTestFixture, ToTimestampIsPositive) {
  const auto timestamp = chronos::to_timestamp(now);
  ASSERT_GT(timestamp, 0);
}