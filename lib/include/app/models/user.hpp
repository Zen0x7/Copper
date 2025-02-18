#pragma once

#include <cstdint>
#include <boost/core/span.hpp>
#include <boost/describe/class.hpp>
#include <boost/optional.hpp>
#include <boost/mysql/datetime.hpp>

#include <copper/components/uuid.hpp>
#include <copper/components/shared.hpp>
#include <utility>

namespace app::models {
    using namespace copper::components;

    class user : public shared_enabled<user> {
    public:
      std::string id_;
      std::string name_;
      std::string email_;
      std::string password_;
      long email_verified_at_;
      long created_at_;
      long updated_at_;

      user(std::string id, std::string name, std::string email, std::string password, long email_verified_at, long created_at, long updated_at) :
        id_(std::move(id)),
        name_(std::move(name)),
        email_(std::move(email)),
        password_(std::move(password)),
        email_verified_at_(email_verified_at),
        created_at_(created_at),
        updated_at_(updated_at)
      {

      }
    };
}