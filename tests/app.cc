#include <gtest/gtest.h>

#include <copper/app.hpp>

using namespace copper;

TEST(App, Version) {
    ASSERT_EQ(get_version(), "2.0.0");
}


