#include <gtest/gtest.h>

#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>
#include <copper/components/validator.hpp>

TEST(Components_Validator, OnSuccess) {
  using namespace copper::components;

  const containers::map_of_strings _rules = {
      {"*", "is_object"},      {"uuid", "is_uuid"},
      {"string", "is_string"}, {"string", "confirmed"},
      {"number", "is_number"}, {"array_of_strings", "is_array_of_strings"},
      {"object", "is_object"},
  };

  const std::string _valid_object =
      R"({"uuid":"7f058c7f-b9e3-4692-a443-e9b317e39424","string":"hello world","string_confirmation":"hello world","number":10,"array_of_strings":["hello","world","!"],"object":{}})";
  const auto _value = boost::json::parse(_valid_object);

  const auto _response = validator_make(_rules, _value);
  ;
  ASSERT_TRUE(_response->success_);
}

TEST(Components_Validator, OnEmptiness) {
  using namespace copper::components;

  const containers::map_of_strings _rules = {
      {"*", "is_object"},      {"uuid", "is_uuid"},
      {"string", "is_string"}, {"string", "confirmed"},
      {"number", "is_number"}, {"array_of_strings", "is_array_of_strings"},
      {"object", "is_object"},
  };

  const std::string _valid_object = R"({})";
  const auto _value = boost::json::parse(_valid_object);

  const auto _response = validator_make(_rules, _value);
  ASSERT_FALSE(_response->success_);

  const std::string _expected_errors =
      R"({"array_of_strings":["Attribute array_of_strings is required."],"number":["Attribute number is required."],"object":["Attribute object is required."],"string":["Attribute string is required."],"uuid":["Attribute uuid is required."]})";
  ASSERT_EQ(serialize(_response->errors_), _expected_errors);
}

TEST(Components_Validator, OnErrors) {
  using namespace copper::components;

  const containers::map_of_strings _rules = {
      {"*", "is_object"},
      {"uuid", "is_string,is_uuid"},
      {"uuid_v4", "is_uuid"},
      {"string", "is_string"},
      {"string", "confirmed"},
      {"strong", "confirmed"},
      {"streng", "confirmed"},
      {"strung", "confirmed"},
      {"number", "is_number"},
      {"array_of_strings", "is_array_of_strings"},
      {"wrong_array_of_strings", "is_array_of_strings"},
      {"empty_array_of_strings", "is_array_of_strings"},
      {"object", "is_object"},
  };

  const std::string _valid_object =
      R"({"uuid":"invalid-uuid","uuid_v4":10,"string":7,"string_confirmation":"hello world","strong":"hello","strong_confirmation":"world","streng":"test","strung":"abc","strung_confirmation":10,"number":"10","array_of_strings":[10,[],"hello"],"wrong_array_of_strings":"abc","empty_array_of_strings":[],"object":[]})";
  const auto _value = boost::json::parse(_valid_object);

  const auto _response = validator_make(_rules, _value);
  ASSERT_FALSE(_response->success_);

  const std::string _expected_errors =
      R"({"array_of_strings":["Attribute array_of_strings at position 0 must be string.","Attribute array_of_strings at position 1 must be string."],"empty_array_of_strings":["Attribute empty_array_of_strings cannot be empty."],"number":["Attribute number must be a number."],"object":["Attribute object must be an object."],"streng":["Attribute streng_confirmation must be present."],"string":["Attribute string must be string."],"strong":["Attribute strong and strong_confirmation must be equals."],"strung":["Attribute strung_confirmation must be string."],"uuid":["Attribute uuid must be uuid."],"uuid_v4":["Attribute uuid_v4 must be string."],"wrong_array_of_strings":["Attribute wrong_array_of_strings must be an array."]})";
  ASSERT_EQ(serialize(_response->errors_), _expected_errors);

  const std::string _invalid_object = R"([1,3,3,7])";
  const auto _invalid_object_value = boost::json::parse(_invalid_object);

  const auto _invalid_object_response =
      validator_make(_rules, _invalid_object_value);
  ASSERT_FALSE(_invalid_object_response->success_);
}
