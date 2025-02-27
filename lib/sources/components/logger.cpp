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
  return boost::shared_ptr<T>(p.get(),
                              boost::bind(&logger_do_release<T>, p, _1));
}

logger::logger(const shared<configuration>& configuration,
               const shared<state>& state)
    : state_(state) {
  const auto _max_size = configuration->get()->logging_max_size_;
  const auto _max_files = configuration->get()->logging_max_files_;
  auto _generator = boost::uuids::random_generator();

  system_ = logger_to_shared(spdlog::rotating_logger_mt(
      to_string(_generator()), "./logs/instance.log", _max_size, _max_files));

  sessions_ = logger_to_shared(spdlog::rotating_logger_mt(
      to_string(_generator()), "./logs/sessions.log", _max_size, _max_files));

  requests_ = logger_to_shared(spdlog::rotating_logger_mt(
      to_string(_generator()), "./logs/requests.log", _max_size, _max_files));

  errors_ = logger_to_shared(spdlog::rotating_logger_mt(
      to_string(_generator()), "./logs/errors.log", _max_size, _max_files));
}

void logger::on_database_error(
    std::string_view where,
    const boost::mysql::error_with_diagnostics& error) const {
  errors_->info("[MySQL] [{}] [{}] [{}] [{}]", where, error.what(),
                error.get_diagnostics().client_message(),
                error.get_diagnostics().server_message());
}
}  // namespace copper::components