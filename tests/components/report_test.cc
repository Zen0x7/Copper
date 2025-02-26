#include <gtest/gtest.h>

#include <copper/components/report.hpp>

TEST(Components_Report, Call) {
  using namespace copper::components;

  constexpr boost::system::error_code _ec;

  report(_ec, "OK");
}
