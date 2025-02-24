#include <copper/components/configuration.hpp>
#include <copper/components/logger.hpp>

namespace copper::components {

logger::logger(shared<configuration> configuration) {
  auto max_size = configuration->get()->logging_max_size_;
  auto max_files = configuration->get()->logging_max_files_;
  system_ = spdlog::rotating_logger_mt("system", "./logs/system.log", max_size,
                                       max_files);
  sessions_ = spdlog::rotating_logger_mt("sessions", "./logs/sessions.log",
                                         max_size, max_files);
  requests_ = spdlog::rotating_logger_mt("requests", "./logs/requests.log",
                                         max_size, max_files);
}
}  // namespace copper::components