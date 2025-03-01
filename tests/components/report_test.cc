//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include <copper/components/report.hpp>

TEST(Components_Report, Call) {
  using namespace copper::components;

  constexpr boost::system::error_code _ec;

  report(_ec, "OK");
}
