#ifndef COPPER_MODELS_SESSION_HPP
#define COPPER_MODELS_SESSION_HPP

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
#include <copper/components/uuid.hpp>

namespace copper::components {

/**
 * Session
 */
class session : public shared_enabled<session> {
 public:
  /**
   * ID
   */
  std::string id_;

  /**
   * IP
   */
  std::string ip_;

  /**
   * Port
   */
  uint_least16_t port_;

  /**
   * Started at
   */
  long started_at_;

  /**
   * Finished at
   */
  long finished_at_;

  /**
   * Constructor
   *
   * @param id
   * @param ip
   * @param port
   * @param started_at
   * @param finished_at
   */
  session(std::string id,
          std::string ip,
          uint_least16_t port,
          long started_at,
          long finished_at)
      : id_(std::move(id)),
        ip_(std::move(ip)),
        port_(port),
        started_at_(started_at),
        finished_at_(finished_at) {}
};

}  // namespace copper::components

#endif
