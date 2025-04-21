#ifndef COPPER_MODELS_USER_HPP
#define COPPER_MODELS_USER_HPP

#pragma once

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

#include <copper/components/shared.hpp>

namespace copper::components {

/**
 * User
 */
class user : public shared_enabled<user> {
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
  user(std::string id,
       std::string name,
       std::string email,
       std::string password,
       const long email_verified_at,
       const long created_at,
       const long updated_at)
      : id_(std::move(id)),
        name_(std::move(name)),
        email_(std::move(email)),
        password_(std::move(password)),
        email_verified_at_(email_verified_at),
        created_at_(created_at),
        updated_at_(updated_at) {}
};

}  // namespace copper::components

#endif
