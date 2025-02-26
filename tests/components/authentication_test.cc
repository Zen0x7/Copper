#include <gtest/gtest.h>

#include <boost/uuid/random_generator.hpp>
#include <copper/components/authentication.hpp>
#include <copper/components/base64.hpp>
#include <copper/components/uuid.hpp>

TEST(Components_Authenticator, IssueAndCheck) {
  using namespace copper::components;

  const uuid _id = boost::uuids::random_generator()();

  const std::string _app_key = "sRgrihyQrBq59ltyxPW/azh9BzVN+vuA/K48BS7nJaw=";

  const std::string _bearer =
      authentication_to_bearer(_id, base64_decode(_app_key), "CI");

  auto _result = authentication_from_bearer(_bearer, base64_decode(_app_key));

  ASSERT_TRUE(_result.has_value());
  ASSERT_EQ(_id, _result.value().id_);
  ASSERT_EQ("CI", _result.value().type_);
}

TEST(Components_Authenticator, Empty) {
  using namespace copper::components;

  const std::string _app_key = "sRgrihyQrBq59ltyxPW/azh9BzVN+vuA/K48BS7nJaw=";

  const std::string _bearer = "Bearer a.b.c";

  const auto _result =
      authentication_from_bearer(_bearer, base64_decode(_app_key));

  ASSERT_FALSE(_result.has_value());
}
