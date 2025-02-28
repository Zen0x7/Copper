#include <gtest/gtest.h>

#include <copper/app.hpp>

using namespace copper;

TEST(App, Version) { ASSERT_EQ(get_version(), "6.0.0"); }
