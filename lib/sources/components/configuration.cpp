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

#include <copper/components/configuration.hpp>

namespace copper::components {
configuration::configuration() {
  dotenv::init();
  values_ = boost::make_shared<configurations>();

  values_->app_debug_ = dotenv::getenv("APP_DEBUG", "ON") == "ON";
  values_->app_logger_ = dotenv::getenv("APP_LOGGER", "ON") == "ON";

  values_->app_host_ = dotenv::getenv("APP_HOST", "0.0.0.0");
  values_->app_key_ = dotenv::getenv("APP_KEY", "secret");
  values_->tcp_port_ = std::stoi(dotenv::getenv("TCP_PORT", "10000"));
  values_->app_port_ = std::stoi(dotenv::getenv("APP_PORT", "9000"));
  values_->websocket_port_ =
      std::stoi(dotenv::getenv("WEBSOCKET_PORT", "11000"));
  values_->app_threads_ = std::stoi(dotenv::getenv("APP_THREADS", "4"));

  values_->database_host_ = dotenv::getenv("DATABASE_HOST", "127.0.0.1");
  values_->database_port_ = std::stoi(dotenv::getenv("DATABASE_PORT", "3306"));
  values_->database_user_ = dotenv::getenv("DATABASE_USER", "user");
  values_->database_password_ =
      dotenv::getenv("DATABASE_PASSWORD", "user_password");
  values_->database_name_ = dotenv::getenv("DATABASE_NAME", "copper");
  values_->database_pool_thread_safe_ =
      dotenv::getenv("DATABASE_POOL_THREAD_SAFE", "ON") == "ON";
  values_->database_pool_initial_size_ =
      std::stoi(dotenv::getenv("DATABASE_POOL_INITIAL_SIZE", "3"));
  values_->database_pool_max_size_ =
      std::stoi(dotenv::getenv("DATABASE_POOL_MAX_SIZE", "100"));

  values_->http_body_limit_ =
      std::stoi(dotenv::getenv("HTTP_BODY_LIMIT", "10000"));
  values_->http_allowed_origins_ = dotenv::getenv("HTTP_ALLOWED_ORIGINS", "*");

  values_->working_directory_ =
      dotenv::getenv("WORKING_DIRECTORY", "/__w/Copper/Copper/bin");
}

shared<configuration> configuration::factory() {
  return boost::make_shared<configuration>();
}
}  // namespace copper::components
