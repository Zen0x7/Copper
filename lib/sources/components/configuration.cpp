#include <copper/components/configuration.hpp>

namespace copper::components {

configuration::configuration() {
  dotenv::init();
  values_ = boost::make_shared<configurations>();

  values_->app_host_ = dotenv::getenv("APP_HOST", "0.0.0.0");
  values_->app_key_ = dotenv::getenv("APP_KEY", "secret");
  values_->app_port_ = std::stoi(dotenv::getenv("APP_PORT", "9000"));
  values_->app_threads_ = std::stoi(dotenv::getenv("APP_THREADS", "4"));
  values_->app_public_key_ =
      dotenv::getenv("APP_PUBLIC_KEY", "./certificates/public.pem");
  values_->app_private_key_ =
      dotenv::getenv("APP_PRIVATE_KEY", "./certificates/private.pem");
  values_->app_dh_params_ =
      dotenv::getenv("APP_DH_PARAMS", "./certificates/params.pem");
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
  values_->redis_host_ = dotenv::getenv("REDIS_HOST", "127.0.0.1");
  values_->redis_port_ = std::stoi(dotenv::getenv("REDIS_PORT", "6379"));
  values_->redis_health_check_interval_ =
      std::stoi(dotenv::getenv("REDIS_HEALTH_CHECK_INTERVAL", "60"));
  values_->redis_connection_timeout_ =
      std::stoi(dotenv::getenv("REDIS_CONNECTION_TIMEOUT", "60"));
  values_->redis_reconnection_wait_interval_ =
      std::stoi(dotenv::getenv("REDIS_RECONNECTION_WAIT_INTERVAL", "3"));
  values_->redis_client_name_ = dotenv::getenv("REDIS_CLIENT_NAME", "copper");
  values_->http_body_limit_ =
      std::stoi(dotenv::getenv("HTTP_BODY_LIMIT", "10000"));
  values_->http_allowed_origins_ = dotenv::getenv("HTTP_ALLOWED_ORIGINS", "*");
}

shared<configurations> configuration::get() { return values_; }
}  // namespace copper::components