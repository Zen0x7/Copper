#include <copper/components/database.hpp>
#include <copper/components/chronos.hpp>

#include <boost/asio/detached.hpp>
#include <boost/asio/cancel_after.hpp>
#include <boost/asio/use_future.hpp>

#include <boost/mysql/results.hpp>
#include <boost/mysql/with_params.hpp>
#include <boost/mysql/with_diagnostics.hpp>
#include <boost/mysql/pool_params.hpp>

#include <boost/uuid/random_generator.hpp>

#include <boost/lexical_cast.hpp>

namespace copper::components {

    void database::start() {
      pool_->async_run(boost::asio::detached);
    }

    /**
     * Find user by email
     *
     * @param email
     * @return
     */
    containers::optional_of<shared<app::models::user>> database::get_user_by_email(const std::string &email) {
      std::chrono::steady_clock::duration timeout = std::chrono::seconds(30);

      auto connection = pool_->async_get_connection(
        boost::mysql::with_diagnostics(boost::asio::cancel_after(timeout, boost::asio::use_future))).get();


      boost::mysql::results result;

      connection->execute(
        boost::mysql::with_params(
          "SELECT id, name, password, email_verified_at, created_at, updated_at FROM users WHERE email = {}",
          email), result);


      if (result.rows().empty()) {
        connection->close();

        return boost::none;
      }

      const auto &row = result.rows().at(0);

      return boost::make_shared<app::models::user>(
        row.at(0).as_string(),
        std::string(row.at(1).as_string()),
        email,
        row.at(2).as_string(),
        row.at(3).is_null() ? 0 : chronos::to_timestamp(row.at(3).as_datetime().as_time_point()),
        row.at(4).is_null() ? 0 : chronos::to_timestamp(row.at(4).as_datetime().as_time_point()),
        row.at(5).is_null() ? 0 : chronos::to_timestamp(row.at(5).as_datetime().as_time_point())
      );
    }

    database::database() {
      thread_pool_ = boost::make_shared<boost::asio::thread_pool>(std::stoi(dotenv::getenv("DATABASE_THREADS", "1")));

      boost::mysql::pool_params params;
      params.server_address.emplace_host_and_port(
        dotenv::getenv("DATABASE_HOST", "127.0.0.1"),
        std::stoi(dotenv::getenv("DATABASE_PORT", "3306"))
      );
      params.username = dotenv::getenv("DATABASE_USER", "user");
      params.password = dotenv::getenv("DATABASE_PASSWORD", "user_password");
      params.database = dotenv::getenv("DATABASE_NAME", "copper");
      params.thread_safe = true;
      params.initial_size = 10;
      params.max_size = 100;

      pool_ = boost::make_shared<boost::mysql::connection_pool>(*thread_pool_, std::move(params));
    }

    /**
     * Find existing user by id
     *
     * @param id
     * @return
     */
    shared<app::models::user> database::get_user_by_id(uuid id) {
      std::chrono::steady_clock::duration timeout = std::chrono::seconds(30);

      auto connection = pool_->async_get_connection(
        boost::mysql::with_diagnostics(boost::asio::cancel_after(timeout, boost::asio::use_future))).get();


      boost::mysql::results result;

      connection->execute(
        boost::mysql::with_params(
          "SELECT id, name, email, password, email_verified_at, created_at, updated_at FROM users WHERE id = {}",
          to_string(id)), result);

      const auto &row = result.rows().at(0);

      connection->close();

      return boost::make_shared<app::models::user>(
        to_string(id),
        std::string(row.at(1).as_string()),
        row.at(2).as_string(),
        row.at(3).as_string(),
        row.at(4).is_null() ? 0 : chronos::to_timestamp(row.at(4).as_datetime().as_time_point()),
        row.at(5).is_null() ? 0 : chronos::to_timestamp(row.at(5).as_datetime().as_time_point()),
        row.at(6).is_null() ? 0 : chronos::to_timestamp(row.at(6).as_datetime().as_time_point())
      );
    }

    shared<app::models::session> database::create_session(const std::string &ip, uint_least16_t port) {
      std::chrono::steady_clock::duration timeout = std::chrono::seconds(30);
      auto now = chronos::now();
      auto id = boost::uuids::random_generator()();

      auto connection = pool_->async_get_connection(
        boost::mysql::with_diagnostics(boost::asio::cancel_after(timeout, boost::asio::use_future))).get();


      boost::mysql::results result;

      connection->execute(
        boost::mysql::with_params(
          "INSERT INTO sessions (id, ip, port, started_at) VALUES ({}, {}, {}, {})",
          to_string(id),
          ip,
          port,
          now
        ), result);

      connection->close();

      return boost::make_shared<app::models::session>(
        to_string(id),
        ip,
        port,
        now,
        0
      );
    }

    void database::session_closed(const shared<app::models::session> & session, const char exception[]) {
      std::chrono::steady_clock::duration timeout = std::chrono::seconds(30);
      auto now = chronos::now();

      auto connection = pool_->async_get_connection(
        boost::mysql::with_diagnostics(boost::asio::cancel_after(timeout, boost::asio::use_future))).get();

      boost::mysql::results result;
      connection->execute(
        boost::mysql::with_params(
          "UPDATE sessions SET finished_at = {}, exception = {} WHERE id = {}",
          now,
          exception,
          session->id_
        ), result);

      connection->close();
    }

    void database::session_is_encrypted(const shared<app::models::session> & session) {
      std::chrono::steady_clock::duration timeout = std::chrono::seconds(30);

      auto connection = pool_->async_get_connection(
        boost::mysql::with_diagnostics(boost::asio::cancel_after(timeout, boost::asio::use_future))).get();

      boost::mysql::results result;
      connection->execute(
        boost::mysql::with_params(
          "UPDATE sessions SET is_encrypted = true WHERE id = {}",
          session->id_
        ), result);


      connection->close();
    }

    void database::session_is_upgrade(const shared<app::models::session> & session) {
      std::chrono::steady_clock::duration timeout = std::chrono::seconds(30);

      auto connection = pool_->async_get_connection(
        boost::mysql::with_diagnostics(boost::asio::cancel_after(timeout, boost::asio::use_future))).get();

      boost::mysql::results result;
      connection->execute(
        boost::mysql::with_params(
          "UPDATE sessions SET is_upgrade = true WHERE id = {}",
          session->id_
        ), result);

      connection->close();
    }
}