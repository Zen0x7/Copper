#include <copper/components/logger.hpp>

namespace copper::components {

logger::logger() {
  auto max_size = 1048576 * 5;
  auto max_files = 3;
  system_ = shared<spdlog::logger>(
      spdlog::rotating_logger_mt("system", "./logs/system.log", max_size,
                                 max_files)
          .get());
  sessions_ = shared<spdlog::logger>(
      spdlog::rotating_logger_mt("sessions", "./logs/sessions.log", max_size,
                                 max_files)
          .get());
  requests_ = shared<spdlog::logger>(
      spdlog::rotating_logger_mt("requests", "./logs/requests.log", max_size,
                                 max_files)
          .get());
}
}  // namespace copper::components