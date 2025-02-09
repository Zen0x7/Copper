#include <gtest/gtest.h>

#include <copper/components/filesystem/normalized_path.hpp>


TEST(Components_Filesystem_NormalizedPath, Usage) {
    using namespace copper::components;

    auto value = filesystem::normalized_path::get("srv/app/bin/", "/../main.cpp");

    ASSERT_EQ(value, "srv/app/bin/../main.cpp");
}