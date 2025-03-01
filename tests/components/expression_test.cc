#include <gtest/gtest.h>

#include <copper/components/expression.hpp>
#include <iostream>

TEST(Components_Expression, Assertions) {
  using namespace copper::components;

  const auto _expression = expression_make("/u/{x}/d");

  ASSERT_FALSE(_expression->get_arguments().empty());
  ASSERT_EQ(_expression->get_arguments().size(), 1);
  ASSERT_EQ(_expression->get_arguments().at(0), "x");

  const auto _non_expression = expression_make("/ping");
  ASSERT_TRUE(_non_expression->get_arguments().empty());
  ASSERT_EQ(_non_expression->get_regex(), "/ping");

  const auto _string_result = _expression->query("/u/c80b/d");
  ASSERT_TRUE(_string_result->matches());
  ASSERT_FALSE(_string_result->get_bindings().empty());
  ASSERT_EQ(_string_result->get("x"), "c80b");

  const auto _integer_result = _expression->query("/u/7/d");

  ASSERT_TRUE(_integer_result->matches());
  ASSERT_FALSE(_integer_result->get_bindings().empty());
  ASSERT_EQ(_integer_result->get("x"), "7");

  const auto _multi_expression = expression_make("/u/{x}/d/{y}/x/{z}");

  ASSERT_FALSE(_multi_expression->get_arguments().empty());
  ASSERT_EQ(_multi_expression->get_arguments().size(), 3);
  ASSERT_EQ(_multi_expression->get_arguments().at(0), "x");
  ASSERT_EQ(_multi_expression->get_arguments().at(1), "y");
  ASSERT_EQ(_multi_expression->get_arguments().at(2), "z");

  const auto _multi_expression_result = _multi_expression->query("/u/c80b/d/5/x/7");

  ASSERT_TRUE(_multi_expression_result->matches());
  ASSERT_FALSE(_multi_expression_result->get_bindings().empty());
  ASSERT_EQ(_multi_expression_result->get("x"), "c80b");
  ASSERT_EQ(_multi_expression_result->get("y"), "5");
  ASSERT_EQ(_multi_expression_result->get("z"), "7");
}
