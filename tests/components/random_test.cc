#include <gtest/gtest.h>

#include <copper/components/random.hpp>

TEST(Components_Random, String) {
    using namespace copper::components;
    const std::string one = random_string(16);
    const std::string two = random_string(32);
    const std::string three = random_string(64);
    const std::string four = random_string(16);
    const std::string five = random_string(32);
    const std::string six = random_string(64);

    ASSERT_NE(one, two);
    ASSERT_NE(two, three);
    ASSERT_NE(three, four);
    ASSERT_NE(four, five);
    ASSERT_NE(one, four);
    ASSERT_NE(two, five);
    ASSERT_NE(three, six);

    ASSERT_EQ(one.size(), four.size());
    ASSERT_EQ(two.size(), five.size());
    ASSERT_EQ(three.size(), six.size());

    ASSERT_EQ(one.size(), 16);
    ASSERT_EQ(two.size(), 32);
    ASSERT_EQ(three.size(), 64);
}
