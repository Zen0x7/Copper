#include <gtest/gtest.h>

#include <copper/components/mime_type.hpp>

TEST(Components_Filesystem_MimeType, Binary) {
  using namespace copper::components;

  const auto _value = mime_type("data.bin");

  ASSERT_EQ(_value, "application/text");
}

TEST(Components_Filesystem_MimeType, Empty) {
  using namespace copper::components;

  const auto _value = mime_type("data");

  ASSERT_EQ(_value, "application/text");
}
