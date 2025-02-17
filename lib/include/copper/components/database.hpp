#pragma once

#include <copper/components/shared.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/dotenv.hpp>
#include <copper/components/report.hpp>

#include <boost/mysql.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mysql/connection_pool.hpp>
#include <boost/mysql/pool_params.hpp>
#include <boost/asio/thread_pool.hpp>

#include <app/models/user.hpp>


namespace copper::components {
    class database : public shared_enabled<database> {
        shared<boost::mysql::connection_pool> pool_;
        shared<boost::asio::thread_pool> thread_pool_;
    public:
        database() {
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

        void start() {
            pool_->async_run(boost::asio::detached);
        }

        containers::optional_of<app::models::user> get_user_by_email(const std::string &email) {
            std::chrono::steady_clock::duration timeout = std::chrono::seconds(30);

            auto connection = pool_->async_get_connection(boost::mysql::with_diagnostics(boost::asio::cancel_after(timeout, boost::asio::use_future))).get();


            boost::mysql::results result;

            connection->execute(
                    boost::mysql::with_params(
                            "SELECT id, name, email_verified_at, password, created_at, updated_at FROM users WHERE email = {}",
                            email), result);



            if (result.rows().empty()) {
                return boost::none;
            }

            const auto &row = result.rows().at(0);

            return app::models::user {
                    .id_ = boost::lexical_cast<uuid>(row.at(0).as_string()),
                    .name_ = row.at(1).as_string(),
                    .email_ = email,
                    .email_verified_at_ = row.at(2).is_null() ? boost::mysql::datetime() : row.at(2).as_datetime(),
                    .password_ = row.at(3).as_string(),
                    .created_at_ = row.at(4).is_null() ? boost::mysql::datetime() : row.at(4).as_datetime(),
                    .updated_at_ = row.at(5).is_null() ? boost::mysql::datetime() : row.at(5).as_datetime()
            };
        }
    };
}