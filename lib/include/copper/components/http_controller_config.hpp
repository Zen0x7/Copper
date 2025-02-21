#ifndef COPPER_COMPONENTS_HTTP_CONTROLLER_CONFIG_HPP
#define COPPER_COMPONENTS_HTTP_CONTROLLER_CONFIG_HPP

#pragma once

namespace copper::components {

struct http_controller_config {
  bool use_auth = false;
  bool use_throttler = false;
  bool use_validator = false;
  bool use_protector = false;
  int rpm = 60;
};

}  // namespace copper::components

#endif