#include <gtest/gtest.h>

#include <copper/components/failures/report.hpp>


TEST(Components_Failure_Report, Print) {
    using namespace copper::components;

    boost::system::error_code ec;

    failure::fail(ec, "OK");
}
