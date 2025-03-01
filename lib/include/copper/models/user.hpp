#ifndef COPPER_MODELS_USER_HPP
#define COPPER_MODELS_USER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/shared.hpp>

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
      : id_(std::move(id)),
        name_(std::move(name)),
        email_(std::move(email)),
        password_(std::move(password)),
        email_verified_at_(email_verified_at),
        created_at_(created_at),
        updated_at_(updated_at) {}
};

}  // namespace copper::models

#endif