#ifndef COPPER_COMPONENTS_LOGGER_HPP
#define COPPER_COMPONENTS_LOGGER_HPP

#pragma once

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

#include <copper/components/shared.hpp>
#include <iostream>

#include "spdlog/sinks/basic_file_sink.h"

namespace copper::components {

/**
 * Forward configuration
 */
class configuration;

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
   * Constructor
   */
  explicit logger(const shared<configuration>& configuration);
};
}  // namespace copper::components

#endif