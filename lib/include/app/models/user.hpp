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
        uuid id_;
        std::string name_;
        std::string email_;
        boost::mysql::datetime email_verified_at_;
        std::string password_;
        boost::mysql::datetime created_at_;
        boost::mysql::datetime updated_at_;
    };
    BOOST_DESCRIBE_STRUCT(user, (), (id_, name_, email_, email_verified_at_, password_, created_at_, updated_at_));
}