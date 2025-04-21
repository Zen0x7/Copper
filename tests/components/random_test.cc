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
#include <copper/components/random.hpp>

using namespace copper::components;

class RandomTestFixture : public ::testing::Test {
 protected:
  std::string s16a, s16b;
  std::string s32a, s32b;
  std::string s64a, s64b;

  void SetUp() override {
    s16a = random_string(16);
    s16b = random_string(16);
    s32a = random_string(32);
    s32b = random_string(32);
    s64a = random_string(64);
    s64b = random_string(64);
  }
};

TEST_F(RandomTestFixture, DifferentOutputs) {
  ASSERT_NE(s16a, s16b);
  ASSERT_NE(s32a, s32b);
  ASSERT_NE(s64a, s64b);
  ASSERT_NE(s16a, s32a);
  ASSERT_NE(s32a, s64a);
  ASSERT_NE(s16b, s64b);
}

TEST_F(RandomTestFixture, CorrectLengths) {
  ASSERT_EQ(s16a.size(), 16);
  ASSERT_EQ(s16b.size(), 16);
  ASSERT_EQ(s32a.size(), 32);
  ASSERT_EQ(s32b.size(), 32);
  ASSERT_EQ(s64a.size(), 64);
  ASSERT_EQ(s64b.size(), 64);
}
