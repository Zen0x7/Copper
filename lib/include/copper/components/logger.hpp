#ifndef COPPER_COMPONENTS_LOGGER_HPP
#define COPPER_COMPONENTS_LOGGER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

#include <boost/mysql/error_with_diagnostics.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {

/**
 * Forward configuration
 */
class configuration;

/**
 * Forward state
 */
class state;

/**
 * Logger
 */
class logger : public shared_enabled<logger> {
 public:
  /**
   * System
   */
  shared<spdlog::logger> system_;

  /**
   * Sessions
   */
  shared<spdlog::logger> sessions_;

  /**
   * Requests
   */
  shared<spdlog::logger> requests_;

  /**
   * Errors
   */
  shared<spdlog::logger> errors_;

  /**
   * Constructor
   */
  explicit logger();

  /**
   * On database error
   *
   * @param where
   * @param error
   */
  void on_database_error(
      std::string_view where,
      const boost::mysql::error_with_diagnostics& error) const;

  /**
   * Get instance
   *
   * @return shared<logger>
   */
  static shared<logger> instance();

 private:
  /**
   * Instance
   */
  static shared<logger> instance_;

  /**
   * Initialization flag
   */
  static std::once_flag initialization_flag_;
};
}  // namespace copper::components

#endif