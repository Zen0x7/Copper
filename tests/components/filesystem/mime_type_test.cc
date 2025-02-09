#include <gtest/gtest.h>

#include <copper/components/filesystem/mime_type.hpp>


TEST(Components_Filesystem_MimeType, Binary) {
    using namespace copper::components;

    auto value = filesystem::mime_type::get("data.bin");

    ASSERT_EQ(value, "application/text");
}

TEST(Components_Filesystem_MimeType, Empty) {
    using namespace copper::components;

    auto value = filesystem::mime_type::get("data");

    ASSERT_EQ(value, "application/text");
}

