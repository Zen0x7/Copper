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
#include <copper/components/expression.hpp>

using namespace copper::components;

class ExpressionTestFixture : public ::testing::Test {
 protected:
  shared<expression> expr_single;
  shared<expression> expr_static;
  shared<expression> expr_multi;

  void SetUp() override {
    expr_single = expression_make("/u/{x}/d");
    expr_static = expression_make("/ping");
    expr_multi = expression_make("/u/{x}/d/{y}/x/{z}");
  }
};

TEST_F(ExpressionTestFixture, ExtractsArgumentsFromPattern) {
  ASSERT_EQ(expr_single->get_arguments().size(), 1);
  ASSERT_EQ(expr_single->get_arguments().at(0), "x");

  ASSERT_EQ(expr_multi->get_arguments().size(), 3);
  ASSERT_EQ(expr_multi->get_arguments().at(0), "x");
  ASSERT_EQ(expr_multi->get_arguments().at(1), "y");
  ASSERT_EQ(expr_multi->get_arguments().at(2), "z");
}

TEST_F(ExpressionTestFixture, StaticExpressionHasNoArguments) {
  ASSERT_TRUE(expr_static->get_arguments().empty());
  ASSERT_EQ(expr_static->get_regex(), "/ping");
}

TEST_F(ExpressionTestFixture, MatchSingleWithString) {
  auto result = expr_single->query("/u/c80b/d");

  ASSERT_TRUE(result->matches());
  ASSERT_EQ(result->get("x"), "c80b");
}

TEST_F(ExpressionTestFixture, MatchSingleWithInteger) {
  auto result = expr_single->query("/u/7/d");

  ASSERT_TRUE(result->matches());
  ASSERT_EQ(result->get("x"), "7");
}

TEST_F(ExpressionTestFixture, MatchMultiPattern) {
  auto result = expr_multi->query("/u/c80b/d/5/x/7");

  ASSERT_TRUE(result->matches());
  ASSERT_EQ(result->get("x"), "c80b");
  ASSERT_EQ(result->get("y"), "5");
  ASSERT_EQ(result->get("z"), "7");
}

TEST_F(ExpressionTestFixture, NonMatchingPatternReturnsNoBindings) {
  auto result = expr_multi->query("/u/c80b/d/5/x");

  ASSERT_FALSE(result->matches());
  ASSERT_TRUE(result->get_bindings().empty());
}

TEST(Components_Expression, EmptyExpression) {
  using namespace copper::components;

  const auto expr = expression_make("");
  ASSERT_TRUE(expr->get_arguments().empty());
  ASSERT_EQ(expr->get_regex(), "");
  const auto result = expr->query("");
  ASSERT_TRUE(result->matches());
}

TEST(Components_Expression, OnlyParameters) {
  using namespace copper::components;

  const auto expr = expression_make("{x}/{y}");
  ASSERT_EQ(expr->get_arguments().size(), 2);
  ASSERT_EQ(expr->get_arguments().at(0), "x");
  ASSERT_EQ(expr->get_arguments().at(1), "y");

  const auto result = expr->query("1/2");
  ASSERT_TRUE(result->matches());
  ASSERT_EQ(result->get("x"), "1");
  ASSERT_EQ(result->get("y"), "2");
}

TEST(Components_Expression, TrailingSlash) {
  using namespace copper::components;

  const auto expr = expression_make("/api/{v}/");
  const auto result = expr->query("/api/1/");
  ASSERT_TRUE(result->matches());
  ASSERT_EQ(result->get("v"), "1");
}

TEST(Components_Expression, Mismatch) {
  using namespace copper::components;

  const auto expr = expression_make("/user/{id}");
  const auto result = expr->query("/device/1");
  ASSERT_FALSE(result->matches());
}

TEST(Components_Expression, DeepNestedMatch) {
  using namespace copper::components;

  const auto expr = expression_make("/a/{x}/b/{y}/c/{z}/d");
  const auto result = expr->query("/a/1/b/2/c/3/d");
  ASSERT_TRUE(result->matches());
  ASSERT_EQ(result->get("x"), "1");
  ASSERT_EQ(result->get("y"), "2");
  ASSERT_EQ(result->get("z"), "3");
}

TEST(Components_Expression, ParameterWithDash) {
  using namespace copper::components;

  const auto expr = expression_make("/device/{device-id}/status");
  const auto result = expr->query("/device/abc-123/status");
  ASSERT_TRUE(result->matches());
  ASSERT_EQ(result->get("device-id"), "abc-123");
}

TEST(Components_Expression, ThrowsWhenDuplicateArgument) {
  using namespace copper::components;

  EXPECT_THROW({ expression_make("/u/{x}/d/{x}"); }, expression_exception);
}

TEST(Components_Expression, ThrowsWhenAccessingMissingBinding) {
  using namespace copper::components;

  const auto expr = expression_make("/user/{id}");
  const auto result = expr->query("/user/123");

  EXPECT_THROW({ result->get("name"); }, expression_exception);
}
