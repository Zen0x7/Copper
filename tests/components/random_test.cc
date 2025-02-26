#include <gtest/gtest.h>

#include <copper/components/random.hpp>

TEST(Components_Random, String) {
  using namespace copper::components;
  const std::string _one = random_string(16);
  const std::string _two = random_string(32);
  const std::string _three = random_string(64);
  const std::string _four = random_string(16);
  const std::string _five = random_string(32);
  const std::string _six = random_string(64);

  ASSERT_NE(_one, _two);
  ASSERT_NE(_two, _three);
  ASSERT_NE(_three, _four);
  ASSERT_NE(_four, _five);
  ASSERT_NE(_one, _four);
  ASSERT_NE(_two, _five);
  ASSERT_NE(_three, _six);

  ASSERT_EQ(_one.size(), _four.size());
  ASSERT_EQ(_two.size(), _five.size());
  ASSERT_EQ(_three.size(), _six.size());

  ASSERT_EQ(_one.size(), 16);
  ASSERT_EQ(_two.size(), 32);
  ASSERT_EQ(_three.size(), 64);
}
