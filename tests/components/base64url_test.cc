#include <gtest/gtest.h>

#include <copper/components/base64url.hpp>

using namespace copper::components;

TEST(Components_Base64url, Encode) {
  const std::string input = "hello+world//";

  const std::string output = base64url_encode(input);
  ASSERT_EQ(output, "aGVsbG8rd29ybGQvLw==");

  const std::string output_no_padding = base64url_encode(input, false);
  ASSERT_EQ(output_no_padding, "aGVsbG8rd29ybGQvLw");
}

TEST(Components_Base64url, Decode) {
  const std::string input = "aGVsbG8rd29ybGQvLw==";

  const std::string output = base64url_decode(input);
  ASSERT_EQ(output, "hello+world//");

  const std::string input_no_padding = "aGVsbG8rd29ybGQvLw";
  const std::string output_no_padding = base64url_decode(input_no_padding);
  ASSERT_EQ(output_no_padding, "hello+world//");
}
