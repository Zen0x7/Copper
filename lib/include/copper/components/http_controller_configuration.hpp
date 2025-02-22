#ifndef COPPER_COMPONENTS_HTTP_CONTROLLER_CONFIG_HPP
#define COPPER_COMPONENTS_HTTP_CONTROLLER_CONFIG_HPP

#pragma once

namespace copper::components {

/**
 * HTTP controller configuration
 */
struct http_controller_configuration {
  /**
   * Use auth
   */
  bool use_auth_ = false;

  /**
   * Use throttler
   */
  bool use_throttler_ = false;

  /**
   * Use validator
   */
  bool use_validator_ = false;

  /**
   * Use protector
   */
  bool use_protector_ = false;

  /**
   * Requests per minute
   */
  int rpm_ = 60;
};

}  // namespace copper::components

#endif