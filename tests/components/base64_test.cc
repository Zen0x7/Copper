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
#include <copper/components/base64.hpp>

using namespace copper::components;

class Base64TestFixture : public ::testing::Test {
 protected:
  const std::string input = "hello+world//";
  const std::string encoded = "aGVsbG8rd29ybGQvLw==";
  const std::string encoded_no_padding = "aGVsbG8rd29ybGQvLw";
};

TEST_F(Base64TestFixture, EncodeWithPadding) {
  ASSERT_EQ(base64_encode(input), encoded);
}

TEST_F(Base64TestFixture, EncodeWithoutPadding) {
  ASSERT_EQ(base64_encode(input, false), encoded_no_padding);
}

TEST_F(Base64TestFixture, DecodeWithPadding) {
  ASSERT_EQ(base64_decode(encoded), input);
}

TEST_F(Base64TestFixture, DecodeWithoutPadding) {
  ASSERT_EQ(base64_decode(encoded_no_padding), input);
}

TEST_F(Base64TestFixture, DecodeEmptyString) {
  ASSERT_EQ(base64_decode(""), "");
}

TEST_F(Base64TestFixture, DecodeInvalidBase64ThrowsOrReturnsEmpty) {
  try {
    std::string result = base64_decode("%%%");  // inválido
    ASSERT_TRUE(result.empty());
  } catch (...) {
    SUCCEED();  // si lanza, también está bien
  }
}