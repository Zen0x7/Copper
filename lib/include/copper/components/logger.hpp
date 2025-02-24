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
 * Logger
 */
class logger : public shared_enabled<logger> {
 public:
  /**
   * Requests
   */
  shared<spdlog::logger> system_;

  /**
   * Requests
   */
  shared<spdlog::logger> sessions_;

  /**
   * Requests
   */
  shared<spdlog::logger> requests_;

  /**
   * Constructor
   */
  logger();
};
}  // namespace copper::components

#endif