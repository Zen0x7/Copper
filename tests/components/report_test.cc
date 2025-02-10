#include <gtest/gtest.h>

#include <copper/components/report.hpp>


TEST(Components_Report, Call) {
    using namespace copper::components;

    boost::system::error_code ec;

    report(ec, "OK");
}
