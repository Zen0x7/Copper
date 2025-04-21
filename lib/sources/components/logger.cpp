// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

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
#include <iostream>
#include <memory>

namespace copper::components {

logger::logger() {
  // const auto _max_size =
  //     std::stoi(dotenv::getenv("LOGGING_MAX_SIZE", "5242880"));
  // const auto _max_files = std::stoi(dotenv::getenv("LOGGING_MAX_FILES",
  // "3")); auto _generator = boost::uuids::random_generator();

  // system_ = spdlog::rotating_logger_mt(
  //     to_string(_generator()), "./logs/instance.log", _max_size, _max_files);
  //
  // sessions_ = spdlog::rotating_logger_mt(
  //     to_string(_generator()), "./logs/sessions.log", _max_size, _max_files);
  //
  // requests_ = spdlog::rotating_logger_mt(
  //     to_string(_generator()), "./logs/requests.log", _max_size, _max_files);
  //
  // errors_ = spdlog::rotating_logger_mt(
  //     to_string(_generator()), "./logs/errors.log", _max_size, _max_files);
}

// LCOV_EXCL_START
void logger::on_database_error(
    std::string_view where,
    const boost::mysql::error_with_diagnostics& error) const {
  LOG("Something went wrong: [" << where << "] " << error.what());
  LOG("Client: [" << where << "] " << error.get_diagnostics().client_message());
  LOG("Server: [" << where << "] " << error.get_diagnostics().server_message());
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
void logger::on_error(const std::string_view where,
                      const boost::system::error_code& error) const {
  LOG("Something went wrong: [" << where << "] " << error.what());
}
// LCOV_EXCL_STOP

shared<logger> logger::factory() {
  return boost::make_shared<logger>();
}
}  // namespace copper::components
