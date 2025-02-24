#ifndef BOOST_BIND_GLOBAL_PLACEHOLDERS
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#endif

#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/logger.hpp>
#include <memory>

namespace copper::components {

template <typename T>
void logger_do_release(typename std::shared_ptr<T> const&, T*) {}

template <typename T>
typename boost::shared_ptr<T> logger_to_shared(
    typename std::shared_ptr<T> const& p) {
  return boost::shared_ptr<T>(
      p.get(), boost::bind(&logger_do_release<T>, p, boost::placeholders::_1));
}

logger::logger(const shared<configuration>& configuration) {
  auto max_size = configuration->get()->logging_max_size_;
  auto max_files = configuration->get()->logging_max_files_;
  auto uuid_generator = boost::uuids::random_generator();

  system_ = logger_to_shared(spdlog::rotating_logger_mt(
      to_string(uuid_generator()), "./logs/instance.log", max_size, max_files));

  sessions_ = logger_to_shared(spdlog::rotating_logger_mt(
      to_string(uuid_generator()), "./logs/sessions.log", max_size, max_files));

  requests_ = logger_to_shared(spdlog::rotating_logger_mt(
      to_string(uuid_generator()), "./logs/requests.log", max_size, max_files));
}
}  // namespace copper::components