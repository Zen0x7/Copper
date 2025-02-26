#include <gtest/gtest.h>

#include <copper/components/base64url.hpp>

using namespace copper::components;

TEST(Components_Base64url, Encode) {
  const std::string _input = "hello+world//";

  const std::string _output = base64url_encode(_input);
  ASSERT_EQ(_output, "aGVsbG8rd29ybGQvLw==");

  const std::string _output_no_padding = base64url_encode(_input, false);
  ASSERT_EQ(_output_no_padding, "aGVsbG8rd29ybGQvLw");
}

TEST(Components_Base64url, Decode) {
  const std::string _input = "aGVsbG8rd29ybGQvLw==";

  const std::string _output = base64url_decode(_input);
  ASSERT_EQ(_output, "hello+world//");

  const std::string _input_no_padding = "aGVsbG8rd29ybGQvLw";
  const std::string _output_no_padding = base64url_decode(_input_no_padding);
  ASSERT_EQ(_output_no_padding, "hello+world//");
}
