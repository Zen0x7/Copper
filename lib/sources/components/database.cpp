#include <boost/asio/cancel_after.hpp>
#include <boost/asio/detached.hpp>
#include <boost/mysql/pool_params.hpp>
#include <boost/mysql/results.hpp>
#include <boost/mysql/with_params.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/database.hpp>
#include <copper/components/logger.hpp>

namespace copper::components {

void database::start() const { pool_->async_run(boost::asio::detached); }

containers::async_of<containers::optional_of<shared<models::user>>>
database::get_user_by_email(const std::string &email) {
  try {
    auto _connection = co_await pool_->async_get_connection(
        boost::asio::cancel_after(std::chrono::seconds(10)));

    boost::mysql::results _result;

    co_await _connection->async_execute(
        boost::mysql::with_params(
            "SELECT id, name, password, email_verified_at, "
            "created_at, updated_at FROM users WHERE email = {}",
            email),
        _result);

    if (_result.rows().empty()) {
      co_return boost::none;
    }

    const auto &_row = _result.rows().at(0);

    co_return boost::make_shared<models::user>(
        _row.at(0).as_string(), std::string(_row.at(1).as_string()), email,
        _row.at(2).as_string(),
        _row.at(3).is_null()
            ? 0
            : chronos::to_timestamp(_row.at(3).as_datetime().as_time_point()),
        _row.at(4).is_null()
            ? 0
            : chronos::to_timestamp(_row.at(4).as_datetime().as_time_point()),
        _row.at(5).is_null()
            ? 0
            : chronos::to_timestamp(_row.at(5).as_datetime().as_time_point()));
  } catch (boost::mysql::error_with_diagnostics &error) {
    logger::instance()->on_database_error("get_user_by_email", error);
    co_return boost::none;
  }
}

database::database(const shared<boost::mysql::connection_pool> &pool)
    : pool_(pool) {}

containers::async_of<containers::optional_of<shared<models::user>>>
database::get_user_by_id(const uuid id) {
  try {
    auto _connection = co_await pool_->async_get_connection(
        boost::asio::cancel_after(std::chrono::seconds(10)));

    boost::mysql::results _result;

    co_await _connection->async_execute(
        boost::mysql::with_params(
            "SELECT id, name, email, password, email_verified_at, created_at, "
            "updated_at FROM users WHERE id = {}",
            to_string(id)),
        _result);

    const auto &_row = _result.rows().at(0);

    co_return boost::make_shared<models::user>(
        to_string(id), _row.at(1).as_string(), _row.at(2).as_string(),
        _row.at(3).as_string(),
        _row.at(4).is_null()
            ? 0
            : chronos::to_timestamp(_row.at(4).as_datetime().as_time_point()),
        _row.at(5).is_null()
            ? 0
            : chronos::to_timestamp(_row.at(5).as_datetime().as_time_point()),
        _row.at(6).is_null()
            ? 0
            : chronos::to_timestamp(_row.at(6).as_datetime().as_time_point()));
  } catch (boost::mysql::error_with_diagnostics &error) {
    logger::instance()->on_database_error("get_user_by_id", error);
    co_return boost::none;
  }
}

containers::async_of<void> database::create_session(const uuid session_id,
                                                    std::string ip,
                                                    uint_least16_t port) {
  try {
    auto _now = chronos::now();

    auto _connection = co_await pool_->async_get_connection(
        boost::asio::cancel_after(std::chrono::seconds(10)));

    boost::mysql::results _result;

    co_await _connection->async_execute(
        boost::mysql::with_params("INSERT INTO sessions (id, ip, port, "
                                  "started_at) VALUES ({}, {}, {}, {})",
                                  to_string(session_id), ip, port, _now),
        _result);
  } catch (boost::mysql::error_with_diagnostics &error) {
    logger::instance()->on_database_error("create_session", error);
  }
}

containers::async_of<void> database::session_closed(const uuid session_id,
                                                    const char exception[]) {
  try {
    auto _now = chronos::now();

    auto _connection = co_await pool_->async_get_connection(
        boost::asio::cancel_after(std::chrono::seconds(10)));

    boost::mysql::results _result;
    co_await _connection->async_execute(
        boost::mysql::with_params("UPDATE sessions SET finished_at = {}, "
                                  "exception = {} WHERE id = {}",
                                  _now, exception, to_string(session_id)),
        _result);
  } catch (boost::mysql::error_with_diagnostics &error) {
    logger::instance()->on_database_error("session_closed", error);
  }
}

containers::async_of<void> database::session_is_upgrade(const uuid session_id) {
  try {
    auto _connection = co_await pool_->async_get_connection(
        boost::asio::cancel_after(std::chrono::seconds(10)));

    boost::mysql::results _result;
    co_await _connection->async_execute(
        boost::mysql::with_params(
            "UPDATE sessions SET is_upgrade = true WHERE id = {}",
            to_string(session_id)),
        _result);
  } catch (boost::mysql::error_with_diagnostics &error) {
    logger::instance()->on_database_error("session_is_upgrade", error);
  }
}

containers::async_of<void> database::create_invocation(
    const shared<models::request> request,
    const shared<models::response> response) {
  try {
    auto _connection = co_await pool_->async_get_connection(
        boost::asio::cancel_after(std::chrono::seconds(10)));

    boost::mysql::results _result;

    auto _request_body = response->protected_ == true ? "" : request->body_;
    auto _response_body = response->protected_ == true ? "" : response->body_;

    co_await _connection->async_execute(
        boost::mysql::with_params(
            "INSERT INTO requests (id, session_id, version, method, path, "
            "query, "
            "headers, body, started_at, finished_at, duration) VALUES ({}, {}, "
            "{}, {}, {}, {}, {}, {}, {}, {}, {})",
            request->id_, request->session_id_, request->version_,
            request->method_, request->path_, request->query_,
            request->headers_, _request_body, request->started_at_,
            request->finished_at_, request->duration_),
        _result);

    co_await _connection->async_execute(
        boost::mysql::with_params(
            "INSERT INTO responses (id, session_id, request_id, status_code, "
            "headers, body) VALUES ({}, {}, {}, {}, {}, {})",
            response->id_, response->session_id_, response->request_id_,
            response->status_code_, response->headers_, _response_body),
        _result);
  } catch (boost::mysql::error_with_diagnostics &error) {
    logger::instance()->on_database_error("create_invocation", error);
  }
}
}  // namespace copper::components