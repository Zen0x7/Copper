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
#include <copper/components/binaries.hpp>
#include <cstring>

using namespace copper::components::binaries;

class BinariesTestFixture : public ::testing::Test {
 protected:
  constexpr static unsigned char test_data[19] = {
      0x12, 0x34, 0x56, 0x78, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
      0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
};

TEST_F(BinariesTestFixture, CRCComputation) {
  ASSERT_EQ(calculate_crc(test_data, 4), 13034);
}

TEST_F(BinariesTestFixture, OneByteExtraction) {
  EXPECT_EQ(one_byte(test_data, 0), 0x12);
  EXPECT_EQ(one_byte(test_data, 1), 0x34);
  EXPECT_EQ(one_byte(test_data, 2), 0x56);
  EXPECT_EQ(one_byte(test_data, 3), 0x78);
}

TEST_F(BinariesTestFixture, TwoByteExtraction) {
  EXPECT_EQ(two_bytes(test_data, 0), 0x1234);
  EXPECT_EQ(two_bytes(test_data, 1), 0x3456);
  EXPECT_EQ(two_bytes(test_data, 2), 0x5678);
}

TEST_F(BinariesTestFixture, FourByteExtraction) {
  EXPECT_EQ(four_bytes(test_data, 0), 0x12345678);
}

TEST_F(BinariesTestFixture, EightByteExtraction) {
  EXPECT_EQ(eight_bytes(test_data, 0), 0x1234567811121314);
}

TEST_F(BinariesTestFixture, NBytesValidRange) {
  const unsigned char* result = n_bytes(test_data, 2, 4);
  ASSERT_NE(result, nullptr);
  EXPECT_TRUE(std::memcmp(result, &test_data[2], 4) == 0);

  result = n_bytes(test_data, 0, 8);
  ASSERT_NE(result, nullptr);
  EXPECT_TRUE(std::memcmp(result, test_data, 8) == 0);
}

TEST_F(BinariesTestFixture, NBytesInvalidRange) {
  const unsigned char* result = n_bytes(test_data, 16, 8);
  EXPECT_EQ(result, nullptr);
}
