#pragma once

#include <cstdint>
#include <boost/core/span.hpp>
#include <boost/describe/class.hpp>
#include <boost/optional.hpp>
#include <boost/mysql/datetime.hpp>

#include <copper/components/uuid.hpp>

namespace app::models {
    using namespace copper::components;

    struct user {
      std::string id_;
      std::string name_;
      std::string email_;
      long email_verified_at_;
      std::string password_;
      long created_at_;
      long updated_at_;
    };
    BOOST_DESCRIBE_STRUCT(user, (), (id_, name_, email_, email_verified_at_, password_, created_at_, updated_at_));
}