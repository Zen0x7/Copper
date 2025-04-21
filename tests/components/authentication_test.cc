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
#include <boost/uuid/random_generator.hpp>
#include <copper/components/authentication.hpp>
#include <copper/components/base64.hpp>
#include <copper/components/uuid.hpp>

using namespace copper::components;

class AuthenticatorTestFixture : public ::testing::Test {
 protected:
  const std::string app_key_base64 =
      "sRgrihyQrBq59ltyxPW/azh9BzVN+vuA/K48BS7nJaw=";
  std::string app_key_decoded;

  void SetUp() override { app_key_decoded = base64_decode(app_key_base64); }
};

TEST_F(AuthenticatorTestFixture, IssueAndCheck) {
  const uuid id = boost::uuids::random_generator()();
  const std::string bearer =
      authentication_to_bearer(id, app_key_decoded, "CI");

  const auto result = authentication_from_bearer(bearer, app_key_decoded);
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(id, result->id_);
  ASSERT_EQ("CI", result->type_);
}

TEST_F(AuthenticatorTestFixture, InvalidBearerReturnsEmpty) {
  const std::string bearer = "Bearer a.b.c";
  const auto result = authentication_from_bearer(bearer, app_key_decoded);
  ASSERT_FALSE(result.has_value());
}

TEST_F(AuthenticatorTestFixture, InvalidSignatureFails) {
  const uuid id = boost::uuids::random_generator()();
  const std::string bearer =
      authentication_to_bearer(id, app_key_decoded, "CI");

  const std::string wrong_key =
      base64_decode("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=");
  const auto result = authentication_from_bearer(bearer, wrong_key);
  ASSERT_FALSE(result.has_value());
}

TEST_F(AuthenticatorTestFixture, EmptyTokenFails) {
  const auto result = authentication_from_bearer("", app_key_decoded);
  ASSERT_FALSE(result.has_value());
}

TEST_F(AuthenticatorTestFixture, WrongTypeIsParsedButDifferent) {
  const uuid id = boost::uuids::random_generator()();
  const std::string bearer =
      authentication_to_bearer(id, app_key_decoded, "WRONG");

  const auto result = authentication_from_bearer(bearer, app_key_decoded);
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(id, result->id_);
  ASSERT_NE("CI", result->type_);
}