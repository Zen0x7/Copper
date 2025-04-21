// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <gtest/gtest.h>
#include <boost/asio/error.hpp>
#include <copper/components/report.hpp>

using namespace copper::components;

class ReportTestFixture : public ::testing::Test {};

TEST_F(ReportTestFixture, CallWithNoError) {
  constexpr boost::system::error_code ec;  // default: no error
  EXPECT_NO_THROW(report(ec, "OK"));
}

TEST_F(ReportTestFixture, CallWithError) {
  const boost::system::error_code ec{boost::asio::error::access_denied};
  EXPECT_NO_THROW(report(ec, "Host lookup"));
}
