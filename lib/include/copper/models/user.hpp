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

/**
 * User
 */
class user : public components::shared_enabled<user> {
 public:
  /**
   * ID
   */
  std::string id_;

  /**
   * Name
   */
  std::string name_;

  /**
   * Email
   */
  std::string email_;

  /**
   * Password
   */
  std::string password_;

  /**
   * Email verified at
   */
  long email_verified_at_;

  /**
   * Created at
   */
  long created_at_;

  /**
   * Updated at
   */
  long updated_at_;

  /**
   * Constructor
   *
   * @param id
   * @param name
   * @param email
   * @param password
   * @param email_verified_at
   * @param created_at
   * @param updated_at
   */
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