#include <gtest/gtest.h>

#include <boost/uuid/random_generator.hpp>
#include <copper/components/authentication.hpp>
#include <copper/components/base64.hpp>

TEST(Components_Authenticator, IssueAndCheck) {
  using namespace copper::components;

  const boost::uuids::uuid id = boost::uuids::random_generator()();

  const std::string app_key = "sRgrihyQrBq59ltyxPW/azh9BzVN+vuA/K48BS7nJaw=";

  const std::string bearer =
      authentication_to_bearer(id, base64_decode(app_key), "CI");

  auto result = authentication_from_bearer(bearer, base64_decode(app_key));

  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(id, result.value().id_);
  ASSERT_EQ("CI", result.value().type_);
}

TEST(Components_Authenticator, Empty) {
  using namespace copper::components;

  const std::string app_key = "sRgrihyQrBq59ltyxPW/azh9BzVN+vuA/K48BS7nJaw=";

  std::string bearer = "Bearer a.b.c";

  auto result = authentication_from_bearer(bearer, base64_decode(app_key));

  ASSERT_FALSE(result.has_value());
}
