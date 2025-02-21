#include <boost/asio/cancel_after.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mysql/pool_params.hpp>
#include <boost/mysql/results.hpp>
#include <boost/mysql/with_diagnostics.hpp>
#include <boost/mysql/with_params.hpp>
#include <boost/uuid/random_generator.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/database.hpp>

namespace copper::components {

void database::start() { pool_->async_run(boost::asio::detached); }

containers::async_of<containers::optional_of<shared<copper::models::user>>>
database::get_user_by_email(const std::string &email) {
  auto connection = co_await pool_->async_get_connection(
      boost::asio::cancel_after(std::chrono::seconds(10)));

  boost::mysql::results result;

  connection->execute(boost::mysql::with_params(
                          "SELECT id, name, password, email_verified_at, "
                          "created_at, updated_at FROM users WHERE email = {}",
                          email),
                      result);

  connection->close();

  if (result.rows().empty()) {
    co_return boost::none;
  }

  const auto &row = result.rows().at(0);

  co_return boost::make_shared<copper::models::user>(
      row.at(0).as_string(), std::string(row.at(1).as_string()), email,
      row.at(2).as_string(),
      row.at(3).is_null()
          ? 0
          : chronos::to_timestamp(row.at(3).as_datetime().as_time_point()),
      row.at(4).is_null()
          ? 0
          : chronos::to_timestamp(row.at(4).as_datetime().as_time_point()),
      row.at(5).is_null()
          ? 0
          : chronos::to_timestamp(row.at(5).as_datetime().as_time_point()));
}

database::database(const shared<boost::mysql::connection_pool> &pool)
    : pool_(pool) {}

containers::async_of<shared<copper::models::user>> database::get_user_by_id(
    uuid id) {
  auto connection = co_await pool_->async_get_connection(
      boost::asio::cancel_after(std::chrono::seconds(10)));

  boost::mysql::results result;

  connection->execute(
      boost::mysql::with_params(
          "SELECT id, name, email, password, email_verified_at, created_at, "
          "updated_at FROM users WHERE id = {}",
          to_string(id)),
      result);

  const auto &row = result.rows().at(0);

  connection->close();

  co_return boost::make_shared<copper::models::user>(
      to_string(id), row.at(1).as_string(), row.at(2).as_string(),
      row.at(3).as_string(),
      row.at(4).is_null()
          ? 0
          : chronos::to_timestamp(row.at(4).as_datetime().as_time_point()),
      row.at(5).is_null()
          ? 0
          : chronos::to_timestamp(row.at(5).as_datetime().as_time_point()),
      row.at(6).is_null()
          ? 0
          : chronos::to_timestamp(row.at(6).as_datetime().as_time_point()));
}

containers::async_of<void> database::create_session(uuid session_id,
                                                    std::string ip,
                                                    uint_least16_t port) {
  auto now = chronos::now();

  auto connection = co_await pool_->async_get_connection(
      boost::asio::cancel_after(std::chrono::seconds(10)));

  boost::mysql::results result;

  connection->execute(
      boost::mysql::with_params("INSERT INTO sessions (id, ip, port, "
                                "started_at) VALUES ({}, {}, {}, {})",
                                to_string(session_id), ip, port, now),
      result);

  connection->close();
}

containers::async_of<void> database::session_closed(uuid session_id,
                                                    const char exception[]) {
  auto now = chronos::now();

  auto connection = co_await pool_->async_get_connection(
      boost::asio::cancel_after(std::chrono::seconds(10)));

  boost::mysql::results result;
  connection->execute(
      boost::mysql::with_params(
          "UPDATE sessions SET finished_at = {}, exception = {} WHERE id = {}",
          now, exception, to_string(session_id)),
      result);

  connection->close();
}

containers::async_of<void> database::session_is_encrypted(uuid session_id) {
  auto connection = co_await pool_->async_get_connection(
      boost::asio::cancel_after(std::chrono::seconds(10)));
  ;

  boost::mysql::results result;
  connection->execute(
      boost::mysql::with_params(
          "UPDATE sessions SET is_encrypted = true WHERE id = {}",
          to_string(session_id)),
      result);

  connection->close();
}

containers::async_of<void> database::session_is_upgrade(uuid session_id) {
  std::chrono::steady_clock::duration timeout = std::chrono::seconds(30);

  auto connection = co_await pool_->async_get_connection(
      boost::asio::cancel_after(std::chrono::seconds(10)));
  ;

  boost::mysql::results result;
  connection->execute(boost::mysql::with_params(
                          "UPDATE sessions SET is_upgrade = true WHERE id = {}",
                          to_string(session_id)),
                      result);

  connection->close();
}

containers::async_of<void> database::create_request(
    shared<copper::models::request> request,
    shared<copper::models::response> response) {
  auto connection = co_await pool_->async_get_connection(
      boost::asio::cancel_after(std::chrono::seconds(10)));

  boost::mysql::results result;

  co_await connection->async_execute(
      boost::mysql::with_params(
          "INSERT INTO requests (id, session_id, version, method, path, query, "
          "headers, body, started_at, finished_at, duration) VALUES ({}, {}, "
          "{}, {}, {}, {}, {}, {}, {}, {}, {})",
          request->id_, request->session_id_, request->version_,
          request->method_, request->path_, request->query_, request->headers_,
          response->protected_ ? "" : request->body_, request->started_at_,
          request->finished_at_, request->duration_),
      result);

  co_await connection->async_execute(
      boost::mysql::with_params(
          "INSERT INTO responses (id, session_id, request_id, status_code, "
          "headers, body) VALUES ({}, {}, {}, {}, {}, {})",
          response->id_, response->session_id_, response->request_id_,
          response->status_code_, response->headers_,
          response->protected_ ? "" : response->body_),
      result);

  connection->close();
}
}  // namespace copper::components