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

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mysql/pool_params.hpp>
#include <boost/mysql/results.hpp>
#include <boost/mysql/with_params.hpp>
#include <copper/components/base64.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/core.hpp>
#include <copper/components/database.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/tracker.hpp>
#include <string>

namespace copper::components {

void database::stop() const {
  std::puts("Database is being stopped ...");
  pool_->cancel();
  std::puts("Database has been stopped ...");
}

void database::get_user_by_email(
    const shared<core>& core,
    const std::string& email,
    callback_of<optional_of<shared<user>>> callback) const {
  LOG("[database@get_user_by_email] scope_in");
  pool_->async_get_connection(
      [core, email, callback = std::move(callback)](
          const boost::system::error_code& connection_error,
          boost::mysql::pooled_connection conn) mutable -> void {
        // LCOV_EXCL_START
        if (connection_error) {
          LOG("[database@get_user_by_email] error on connection: "
              << connection_error.what());
          core->logger_->on_error("get_user_by_email (connection)",
                                  connection_error);
          callback(boost::none);
          return;
        }
        // LCOV_EXCL_STOP

        auto _result = boost::make_shared<boost::mysql::results>();
        auto constexpr _query =
            "SELECT id, name, password, email_verified_at, "
            "created_at, updated_at FROM users WHERE email = "
            "{}";

        conn->async_execute(
            boost::mysql::with_params(_query, email), *_result,
            [conn = std::move(conn), core, email, _query, _result,
             callback = std::move(callback)](
                const boost::system::error_code& execute_error) mutable
            -> void {
              // LCOV_EXCL_START
              if (execute_error) {
                LOG("[database@get_user_by_email] error on execute: "
                    << execute_error.what());
                core->logger_->on_error("get_user_by_email (query)",
                                        execute_error);
                callback(boost::none);
                return;
              }
              // LCOV_EXCL_STOP

              if (_result->rows().empty()) {
                callback(boost::none);
                return;
              }

              const auto& _row = _result->rows().at(0);

              auto _id = _row.at(0).as_string();
              auto _name = std::string(_row.at(1).as_string());
              auto _password = _row.at(2).as_string();
              // LCOV_EXCL_START
              auto _email_verified_at =
                  _row.at(3).is_null()
                      ? 0
                      : chronos::to_timestamp(
                            _row.at(3).as_datetime().as_time_point());
              auto _created_at =
                  _row.at(4).is_null()
                      ? 0
                      : chronos::to_timestamp(
                            _row.at(4).as_datetime().as_time_point());
              auto _updated_at =
                  _row.at(5).is_null()
                      ? 0
                      : chronos::to_timestamp(
                            _row.at(5).as_datetime().as_time_point());
              // LCOV_EXCL_STOP

              callback(boost::make_shared<user>(_id, _name, email, _password,
                                                _email_verified_at, _created_at,
                                                _updated_at));
            });
      });

  LOG("[database@get_user_by_email] scope_out");
}

void database::get_user_by_id(
    const shared<core>& core,
    const uuid& id,
    callback_of<optional_of<shared<user>>> callback) const {
  LOG("[database@get_user_by_id] scope_in");

  pool_->async_get_connection(
      [core, id, callback = std::move(callback)](
          const boost::system::error_code& connection_error,
          boost::mysql::pooled_connection conn) mutable -> void {
        LOG("[database@get_user_by_id] pool_.async_get_connection scope_in");

        // LCOV_EXCL_START
        if (connection_error) {
          LOG("[database@get_user_by_id] error on connection: "
              << connection_error.message());
          core->logger_->on_error("get_user_by_id (connection)",
                                  connection_error);
          callback(boost::none);
          return;
        }
        // LCOV_EXCL_STOP

        LOG("[database@get_user_by_id] id: " << to_string(id));

        auto _result = boost::make_shared<boost::mysql::results>();
        auto constexpr _query =
            "SELECT id, name, email, password, "
            "email_verified_at, created_at, updated_at FROM "
            "users WHERE id = {}";

        conn->async_execute(
            boost::mysql::with_params(_query, to_string(id)), *_result,
            [conn = std::move(conn), core, id, _query, _result,
             callback = std::move(callback)](
                const boost::system::error_code& execute_error) mutable
            -> void {
              // LCOV_EXCL_START
              if (execute_error) {
                LOG("[database@get_user_by_id] error on execute: "
                    << execute_error.message());
                core->logger_->on_error("get_user_by_id (query)",
                                        execute_error);
                callback(boost::none);
                return;
              }

              if (!_result->has_value() || _result->rows().empty()) {
                LOG("[database@get_user_by_id] empty has_value: "
                    << _result->has_value());
                callback(boost::none);
                return;
              }
              // LCOV_EXCL_STOP

              const auto& _row = _result->rows().at(0);

              auto _name = std::string(_row.at(1).as_string());
              auto _email = _row.at(2).as_string();
              auto _password = _row.at(3).as_string();
              auto _email_verified_at =
                  _row.at(4).is_null()
                      ? 0
                      : chronos::to_timestamp(
                            _row.at(4).as_datetime().as_time_point());
              auto _created_at =
                  _row.at(5).is_null()
                      ? 0
                      : chronos::to_timestamp(
                            _row.at(5).as_datetime().as_time_point());
              auto _updated_at =
                  _row.at(6).is_null()
                      ? 0
                      : chronos::to_timestamp(
                            _row.at(6).as_datetime().as_time_point());

              callback(boost::make_shared<user>(to_string(id), _name, _email,
                                                _password, _email_verified_at,
                                                _created_at, _updated_at));
            });

        LOG("[database@get_user_by_id] pool_.async_get_connection scope_out");
      });
  LOG("[database@get_user_by_id] scope_out");
}

void database::create_connection(const shared<core>& core,
                                 const shared<connection>& connection) const {
  LOG("[database@create_connection] scope_in");
  pool_->async_get_connection(
      [core, connection](const boost::system::error_code& connection_error,
                         boost::mysql::pooled_connection conn) mutable -> void {
        auto _now = chronos::now();

        // LCOV_EXCL_START
        if (connection_error) {
          LOG("[database@create_connection] error on connection: "
              << connection_error.what());
          core->logger_->on_error("create_connection (connection)",
                                  connection_error);
          return;
        }  // LCOV_EXCL_STOP

        auto _result = boost::make_shared<boost::mysql::results>();
        auto constexpr _query =
            "INSERT INTO connections (id, ip, port, "
            "connected_at) VALUES ({}, {}, {}, {})";

        conn->async_execute(
            boost::mysql::with_params(_query, to_string(connection->id_),
                                      connection->ip_, connection->port_, _now),
            *_result,
            [conn = std::move(conn), core, connection, _query,
             _result](const boost::system::error_code& execute_error) mutable
            -> void {
              // LCOV_EXCL_START
              if (execute_error) {
                LOG("[database@create_connection] error on execute: "
                    << execute_error.what());
                core->logger_->on_error("create_connection (query)",
                                        execute_error);
              }
              // LCOV_EXCL_STOP
            });
      });
  LOG("[database@create_connection] scope_out");
}

void database::connection_closed(const shared<core>& core,
                                 const uuid connection_id,
                                 const char exception[]) const {
  LOG("[database@connection_closed] scope_in");
  pool_->async_get_connection(
      [core, connection_id, exception](
          const boost::system::error_code& connection_error,
          boost::mysql::pooled_connection conn) mutable -> void {
        auto _now = chronos::now();

        // LCOV_EXCL_START
        if (connection_error) {
          LOG("[database@connection_closed] error on connection: "
              << connection_error.what());
          core->logger_->on_error("connection_closed (connection)",
                                  connection_error);
          return;
        }
        // LCOV_EXCL_STOP

        auto _result = boost::make_shared<boost::mysql::results>();
        auto constexpr _query =
            "UPDATE connections SET disconnected_at = {}, "
            "exception = {} WHERE id = {}";

        conn->async_execute(
            boost::mysql::with_params(_query, _now, exception,
                                      to_string(connection_id)),
            *_result,
            [conn = std::move(conn), core, _now, exception, connection_id,
             _query,
             _result](const boost::system::error_code& execute_error) mutable
            -> void {
              // LCOV_EXCL_START
              if (execute_error) {
                LOG("[database@connection_closed] error on execute: "
                    << execute_error.what());
                core->logger_->on_error("connection_closed (query)",
                                        execute_error);
              }
              // LCOV_EXCL_STOP
            });
      });
  LOG("[database@connection_closed] scope_out");
}

void database::connection_is_websocket(const shared<core>& core,
                                       const uuid connection_id) const {
  LOG("[database@connection_is_websocket] scope_in");
  pool_->async_get_connection(
      [core, connection_id](
          const boost::system::error_code& connection_error,
          boost::mysql::pooled_connection conn) mutable -> void {
        // LCOV_EXCL_START
        if (connection_error) {
          LOG("[database@connection_is_websocket] error on connection: "
              << connection_error.what());
          core->logger_->on_error("connection_is_websocket (connection)",
                                  connection_error);
        }
        // LCOV_EXCL_STOP

        auto _protocol = "WS";
        auto _result = boost::make_shared<boost::mysql::results>();
        auto constexpr _query =
            "UPDATE connections SET "
            "protocol = {} WHERE id = {}";

        conn->async_execute(
            boost::mysql::with_params(_query, _protocol,
                                      to_string(connection_id)),
            *_result,
            [conn = std::move(conn), core, connection_id, _protocol, _query,
             _result](const boost::system::error_code& execute_error) mutable
            -> void {
              // LCOV_EXCL_START
              if (execute_error) {
                LOG("[database@connection_is_websocket] error on execute: "
                    << execute_error.what());
                core->logger_->on_error("connection_is_websocket (query)",
                                        execute_error);
              }
              // LCOV_EXCL_STOP
            });
      });
  LOG("[database@connection_is_websocket] scope_out");
}

void database::create_invocation(const shared<core>& core,
                                 const shared<request>& request,
                                 const shared<response>& response) const {
  LOG("[database@create_invocation] scope_in");
  pool_->async_get_connection(
      [core, request, response](
          const boost::system::error_code& connection_error,
          boost::mysql::pooled_connection conn) mutable -> void {
        // LCOV_EXCL_START
        if (connection_error) {
          LOG("[database@create_invocation] error on connection (request): "
              << connection_error.what());
          core->logger_->on_error("create_invocation request (connection)",
                                  connection_error);
          return;
        }
        // LCOV_EXCL_STOP

        auto _create_request_result =
            boost::make_shared<boost::mysql::results>();

        auto _request_body = response->protected_ == true ? "" : request->body_;

        const auto req_id = to_string(request->id_);
        const auto conn_id = to_string(request->connection_->id_);
        const auto version = request->version_;
        const auto method = request->method_;
        const auto path = request->path_;
        const auto query = request->query_;
        const auto headers = request->headers_;
        const auto started_at = request->started_at_;
        const auto finished_at = request->finished_at_;
        const auto duration = request->duration_;

        auto constexpr _requests_query =
            "INSERT INTO requests (id, connection_id, version, "
            "method, path, query, headers, body, started_at, "
            "finished_at, duration) VALUES ({}, {}, {}, {}, "
            "{}, {}, {}, {}, {}, {}, {})";

        conn->async_execute(
            boost::mysql::with_params(
                _requests_query, req_id, conn_id, version, method, path, query,
                headers, _request_body, started_at, finished_at, duration),
            *_create_request_result,
            [conn = std::move(conn), core, req_id, conn_id, version, method,
             path, query, headers, _request_body, started_at, finished_at,
             duration, _requests_query, _create_request_result](
                const boost::system::error_code& execute_error) mutable
            -> void {
              // LCOV_EXCL_START
              if (execute_error) {
                LOG("[database@create_invocation] error on execute "
                    "(create_request): "
                    << execute_error.what());
                core->logger_->on_error(
                    "create_invocation (query: create_request)", execute_error);
              }
              // LCOV_EXCL_STOP
            });
      });

  pool_->async_get_connection(
      [core, response](const boost::system::error_code& connection_error,
                       boost::mysql::pooled_connection conn) mutable -> void {
        // LCOV_EXCL_START
        if (connection_error) {
          LOG("[database@create_invocation] error on connection (response): "
              << connection_error.what());
          core->logger_->on_error("create_invocation response (connection)",
                                  connection_error);
          return;
        }
        // LCOV_EXCL_STOP

        auto _create_response_result =
            boost::make_shared<boost::mysql::results>();

        const auto _response_body =
            response->protected_ == true ? "" : response->body_;
        auto _encoded_response_body = response->compressed_
                                          ? base64_encode(_response_body)
                                          : _response_body;

        const auto res_id = to_string(response->id_);
        const auto res_conn_id = to_string(response->connection_id_);
        const auto res_req_id = to_string(response->request_id_);
        const auto status_code = response->status_code_;
        const auto res_headers = response->headers_;

        auto constexpr _responses_query =
            "INSERT INTO responses (id, connection_id, "
            "request_id, status_code, headers, "
            "body) VALUES ({}, {}, {}, {}, {}, {})";

        conn->async_execute(
            boost::mysql::with_params(_responses_query, res_id, res_conn_id,
                                      res_req_id, status_code, res_headers,
                                      _encoded_response_body),
            *_create_response_result,
            [conn = std::move(conn), res_id, res_conn_id, res_req_id,
             status_code, res_headers, _encoded_response_body, core,
             _responses_query, _create_response_result](
                const boost::system::error_code& execute_error) mutable
            -> void {
              // LCOV_EXCL_START
              if (execute_error) {
                LOG("[database@create_invocation] error on execute "
                    "(create_response): "
                    << execute_error.what());
                core->logger_->on_error(
                    "create_invocation (query: create_response)",
                    execute_error);
              }
              // LCOV_EXCL_STOP
            });
      });
  LOG("[database@create_invocation] scope_out");
}

void database::find_tracker(
    const shared<core>& core,
    const std::string& imei,
    callback_of<optional_of<shared<tracker>>> callback) const {
  LOG("[database@find_tracker] scope_in");
  pool_->async_get_connection(
      [core, imei, callback = std::move(callback)](
          const boost::system::error_code& connection_error,
          boost::mysql::pooled_connection conn) mutable -> void {
        // LCOV_EXCL_START
        if (connection_error) {
          LOG("[database@find_tracker] error on connection: "
              << connection_error.what());
          core->logger_->on_error("find_tracker (connection)",
                                  connection_error);
          callback(boost::none);
          return;
        }
        // LCOV_EXCL_STOP

        auto _result = boost::make_shared<boost::mysql::results>();
        auto constexpr _query = "SELECT id FROM trackers WHERE imei = {}";

        conn->async_execute(
            boost::mysql::with_params(_query, imei), *_result,
            [conn = std::move(conn), core, imei, _query, _result,
             callback = std::move(callback)](
                const boost::system::error_code& execute_error) mutable
            -> void {
              // LCOV_EXCL_START
              if (execute_error) {
                LOG("[database@find_tracker] error on execute: "
                    << execute_error.what());
                core->logger_->on_error("find_tracker (query)", execute_error);
                callback(boost::none);
                return;
              }
              // LCOV_EXCL_STOP

              LOG("[database@find_tracker] imei: " << imei);

              // LCOV_EXCL_START
              if (!_result->has_value() || _result->rows().size() == 0) {
                callback(boost::none);
                return;
              }
              // LCOV_EXCL_STOP

              const auto& _row = _result->rows().at(0);
              auto _id = boost::lexical_cast<uuid>(_row.at(0).as_string());

              callback(boost::make_shared<tracker>(_id, imei));
            });
      });
  LOG("[database@find_tracker] scope_out");
}

void database::create_user(const boost::shared_ptr<core>& core,
                           const std::string& name,
                           const std::string& email,
                           const std::string& password,
                           callback_of<bool> callback) const {
  LOG("[database@create_user] scope_in");
  pool_->async_get_connection(
      [core, name, email, password, callback = std::move(callback)](
          const boost::system::error_code& connection_error,
          boost::mysql::pooled_connection conn) mutable -> void {
        // LCOV_EXCL_START
        if (connection_error) {
          LOG("[database@create_user] error on connection: "
              << connection_error.what());
          core->logger_->on_error("create_user (connection)", connection_error);
          callback(false);
          return;
        }
        // LCOV_EXCL_STOP

        const auto _id = boost::uuids::random_generator()();
        const auto _password = cipher_password_hash(password);

        auto _result = boost::make_shared<boost::mysql::results>();
        auto constexpr _query =
            "INSERT INTO users (id, name, email, "
            "password) VALUES ({}, {}, {}, {})";

        conn->async_execute(
            boost::mysql::with_params(_query, to_string(_id), name, email,
                                      _password),
            *_result,
            [conn = std::move(conn), core, _id, name, email, _password, _query,
             _result, callback = std::move(callback)](
                const boost::system::error_code& execute_error) mutable
            -> void {
              // LCOV_EXCL_START
              if (execute_error) {
                LOG("[database@create_user] error on execute: "
                    << execute_error.what());
                core->logger_->on_error("create_user (query)", execute_error);
                callback(false);
                return;
              }

              callback(true);
              // LCOV_EXCL_STOP
            });
      });
  LOG("[database@create_user] scope_out");
}

shared<database> database::factory() {
  return boost::make_shared<database>();
}

void database::init(boost::asio::io_context& ioc, const shared<core>& core) {
  std::puts("Database is being initialized ... ");

  boost::mysql::pool_params _params;

  std::puts("DB_HOST: ");
  std::puts(core->configuration_->get()->database_host_.data());

  std::puts("DB_PORT: ");
  std::puts(std::to_string(core->configuration_->get()->database_port_).data());

  // LCOV_EXCL_START
  _params.server_address.emplace_host_and_port(
      core->configuration_->get()->database_host_,
      core->configuration_->get()->database_port_);
  // LCOV_EXCL_STOP

  _params.username = core->configuration_->get()->database_user_;
  _params.password = core->configuration_->get()->database_password_;
  _params.database = core->configuration_->get()->database_name_;

  std::puts("DB_THREADS_SAFE: ");
  std::puts(
      std::to_string(core->configuration_->get()->database_pool_thread_safe_)
          .data());

  std::puts("DB_INITIAL_SIZE: ");
  std::puts(
      std::to_string(core->configuration_->get()->database_pool_initial_size_)
          .data());

  std::puts("DB_MAX_SIZE: ");
  std::puts(std::to_string(core->configuration_->get()->database_pool_max_size_)
                .data());

  _params.thread_safe = core->configuration_->get()->database_pool_thread_safe_;
  _params.initial_size =
      core->configuration_->get()->database_pool_initial_size_;
  _params.max_size = core->configuration_->get()->database_pool_max_size_;

  pool_ = boost::make_shared<boost::mysql::connection_pool>(ioc,
                                                            std::move(_params));

  pool_->async_run(boost::asio::detached);

  core->database_ready_ = true;

  std::puts("Database has been initialized ... ");
}
}  // namespace copper::components
