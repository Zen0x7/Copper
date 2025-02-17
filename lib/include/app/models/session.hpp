#pragma once

#include <cstdint>
#include <boost/core/span.hpp>
#include <boost/describe/class.hpp>
#include <boost/optional.hpp>
#include <boost/mysql/datetime.hpp>

#include <copper/components/uuid.hpp>

namespace app::models {
    using namespace copper::components;

    struct session {
      std::string id_;
      std::string ip_;
      uint_least16_t port_;
      long started_at_;
      long finished_at_;
    };
    BOOST_DESCRIBE_STRUCT(session, (), (id_, ip_, port_, started_at_, finished_at_));
}