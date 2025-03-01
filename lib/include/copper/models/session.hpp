#ifndef COPPER_MODELS_SESSION_HPP
#define COPPER_MODELS_SESSION_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

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
 * Session
 */
class session : public components::shared_enabled<session> {
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
  session(std::string id, std::string ip, uint_least16_t port, long started_at,
          long finished_at)
      : id_(std::move(id)),
        ip_(std::move(ip)),
        port_(port),
        started_at_(started_at),
        finished_at_(finished_at) {}
};

}  // namespace copper::models

#endif