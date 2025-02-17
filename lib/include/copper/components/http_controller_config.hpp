#pragma once

namespace copper::components {
    struct http_controller_config {
      bool use_auth = false;
      bool use_throttler = false;
      bool use_validator = false;
      int rpm = 60;
    };
}