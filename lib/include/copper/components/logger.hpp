#ifndef COPPER_COMPONENTS_LOGGER_HPP
#define COPPER_COMPONENTS_LOGGER_HPP

#pragma once

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

#include <boost/mysql/error_with_diagnostics.hpp>
#include <copper/components/shared.hpp>

#include "spdlog/sinks/basic_file_sink.h"

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
  explicit logger(const shared<configuration>& configuration);

  /**
   * On database error
   *
   * @param where
   * @param error
   */
  void on_database_error(
      std::string_view where,
      const boost::mysql::error_with_diagnostics& error) const;
};
}  // namespace copper::components

#endif