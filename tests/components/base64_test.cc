#include <gtest/gtest.h>

#include <copper/components/base64.hpp>

using namespace copper::components;

TEST(Components_Base64, Encode) {
    const std::string input = "hello+world//";

    const std::string output = base64::encode(input);
    ASSERT_EQ(output, "aGVsbG8rd29ybGQvLw==");

    const std::string output_no_padding = base64::encode(input, false);
    ASSERT_EQ(output_no_padding, "aGVsbG8rd29ybGQvLw");
}

TEST(Components_Base64, Decode) {
    const std::string input = "aGVsbG8rd29ybGQvLw==";

    const std::string output = base64::decode(input);
    ASSERT_EQ(output, "hello+world//");

    const std::string input_no_padding = "aGVsbG8rd29ybGQvLw";
    const std::string output_no_padding = base64::decode(input_no_padding);
    ASSERT_EQ(output_no_padding, "hello+world//");
}

