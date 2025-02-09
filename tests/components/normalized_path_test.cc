#include <gtest/gtest.h>

#include <copper/components/normalized_path.hpp>


TEST(Components_Filesystem_NormalizedPath, Usage) {
    using namespace copper::components;

    auto value = normalized_path("srv/app/bin/", "/../main.cpp");

    ASSERT_EQ(value, "srv/app/bin/../main.cpp");
}