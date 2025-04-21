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
#include <copper/components/normalized_path.hpp>

using namespace copper::components;

class NormalizedPathTestFixture : public ::testing::Test {
 protected:
  void expect_path(const std::string& base,
                   const std::string& path,
                   const std::string& expected) {
    ASSERT_EQ(normalized_path(base, path), expected);
  }
};

TEST_F(NormalizedPathTestFixture, BasicConcatenation) {
  expect_path("srv/app/bin/", "/../main.cpp", "srv/app/bin/../main.cpp");
}

TEST_F(NormalizedPathTestFixture, PathWithoutLeadingSlash) {
  expect_path("srv/app/", "main.cpp", "srv/app/main.cpp");
}

TEST_F(NormalizedPathTestFixture, PathWithExtraSlashes) {
  expect_path("srv/app//", "/./file.txt", "srv/app//./file.txt");
}

TEST_F(NormalizedPathTestFixture, BaseWithoutTrailingSlash) {
  expect_path("srv/app", "/hello.cpp", "srv/app/hello.cpp");
}

TEST_F(NormalizedPathTestFixture, BaseEmpty) {
  expect_path("", "/main.cpp", "/main.cpp");
  expect_path("", "main.cpp", "main.cpp");
}
