#ifndef COPPER_COMPONENTS_CORE_HPP
#define COPPER_COMPONENTS_CORE_HPP

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

#include <copper/components/cache.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/database.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/router.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/views.hpp>

namespace copper::components {
/**
 * Forward state
 */
class state;

/**
 * Core
 */
class core : public shared_enabled<core> {
 public:
  /**
   * Cache
   */
  shared<cache> cache_;

  /**
   * State
   */
  shared<state> state_;

  /**
   * Logger
   */
  shared<logger> logger_;

  /**
   * Database
   */
  shared<database> database_;

  /**
   * Configuration
   */
  shared<configuration> configuration_;

  /**
   * Router
   */
  shared<router> router_;

  /**
   * Views
   */
  shared<views> views_;

  /**
   * TCP ready status
   */
  std::atomic_bool tcp_ready_ = false;

  /**
   * HTTP ready status
   */
  std::atomic_bool http_ready_ = false;

  /**
   * Websocket ready status
   */
  std::atomic_bool websocket_ready_ = false;

  /**
   * Database ready status
   */
  std::atomic_bool database_ready_ = false;

  /**
   * Subscriber ready status
   */
  std::atomic_bool subscriber_ready_ = false;

  /**
   * Constructor
   *
   * @param cache
   * @param state
   * @param logger
   * @param database
   * @param configuration
   * @param router
   * @param views
   */
  core(const shared<cache>& cache,
       const shared<state>& state,
       const shared<logger>& logger,
       const shared<database>& database,
       const shared<configuration>& configuration,
       const shared<router>& router,
       const shared<views>& views);

  /**
   * Factory
   *
   * @param ioc
   */
  static shared<core> factory(boost::asio::io_context& ioc);
};
}  // namespace copper::components

#endif
