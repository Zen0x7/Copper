#ifndef COPPER_MODELS_USER_HPP
#define COPPER_MODELS_USER_HPP

#pragma once

#include <boost/core/span.hpp>
#include <boost/describe/class.hpp>
#include <boost/mysql/datetime.hpp>
#include <boost/optional.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>
#include <cstdint>
#include <utility>

namespace copper::models {

class user : public components::shared_enabled<user> {
 public:
  std::string id_;
  std::string name_;
  std::string email_;
  std::string password_;
  long email_verified_at_;
  long created_at_;
  long updated_at_;

  user(std::string id, std::string name, std::string email,
       std::string password, long email_verified_at, long created_at,
       long updated_at)
      : id_(id),
        name_(name),
        email_(email),
        password_(password),
        email_verified_at_(email_verified_at),
        created_at_(created_at),
        updated_at_(updated_at) {}
};

}  // namespace copper::models

#endif