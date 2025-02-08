#include <gtest/gtest.h>
#include <copper/components/validator.hpp>


TEST(Components_Validator, OnSuccess) {
    using namespace copper::components;

    const std::map<std::string, std::string> rules = {
            {"*",                "is_object"},
            {"uuid",             "is_uuid"},
            {"string",           "is_string"},
            {"string",           "confirmed"},
            {"number",           "is_number"},
            {"array_of_strings", "is_array_of_strings"},
            {"object",           "is_object"},
    };

    const std::string valid_object = R"({"uuid":"7f058c7f-b9e3-4692-a443-e9b317e39424","string":"hello world","string_confirmation":"hello world","number":10,"array_of_strings":["hello","world","!"],"object":{}})";
    const auto value = boost::json::parse(valid_object);

    const auto response = validator::make(rules, value);;
    ASSERT_TRUE(response->success);
}

TEST(Components_Validator, OnEmptiness) {
    using namespace copper::components;

    const std::map<std::string, std::string> rules = {
            {"*",                "is_object"},
            {"uuid",             "is_uuid"},
            {"string",           "is_string"},
            {"string",           "confirmed"},
            {"number",           "is_number"},
            {"array_of_strings", "is_array_of_strings"},
            {"object",           "is_object"},
    };

    const std::string valid_object = R"({})";
    const auto value = boost::json::parse(valid_object);

    const auto response = validator::make(rules, value);
    ASSERT_FALSE(response->success);

    const std::string expected_errors = R"({"array_of_strings":["Attribute array_of_strings is required."],"number":["Attribute number is required."],"object":["Attribute object is required."],"string":["Attribute string is required."],"uuid":["Attribute uuid is required."]})";
    ASSERT_EQ(serialize(response->errors), expected_errors);
}

TEST(Components_Validator, OnErrors) {
    using namespace copper::components;

    const std::map<std::string, std::string> rules = {
            {"*",                "is_object"},
            {"uuid",             "is_uuid"},
            {"string",           "is_string"},
            {"string",           "confirmed"},
            {"strong",           "confirmed"},
            {"number",           "is_number"},
            {"array_of_strings", "is_array_of_strings"},
            {"object",           "is_object"},
    };

    const std::string valid_object = R"({"uuid":"invalid-uuid","string":7,"string_confirmation":"hello world","strong":"hello","strong_confirmation":"world","number":"10","array_of_strings":[10,[],"hello"],"object":[]})";
    const auto value = boost::json::parse(valid_object);

    const auto response = validator::make(rules, value);
    ASSERT_FALSE(response->success);

    const std::string expected_errors = R"({"array_of_strings":["Attribute array_of_strings at position 0 must be string.","Attribute array_of_strings at position 1 must be string."],"number":["Attribute number must be a number."],"object":["Attribute object must be an object."],"string":["Attribute string must be string."],"strong":["Attribute strong and strong_confirmation must be equals."],"uuid":["Attribute uuid must be uuid."]})";
    ASSERT_EQ(serialize(response->errors), expected_errors);
}

