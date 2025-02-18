#pragma once

#include <copper/components/shared.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/dotenv.hpp>
#include <copper/components/report.hpp>
#include <copper/components/uuid.hpp>

#include <app/models/user.hpp>
#include <app/models/session.hpp>

#include <boost/mysql/connection_pool.hpp>
#include <boost/asio/thread_pool.hpp>

namespace copper::components {
    class database : public shared_enabled<database> {
        shared<boost::mysql::connection_pool> pool_;
        shared<boost::asio::thread_pool> thread_pool_;
    public:
        database();

        ~database() {
          pool_->cancel();
          thread_pool_->stop();
        }

        void start();

        containers::optional_of<shared<app::models::user>> get_user_by_email(const std::string &email);

        shared<app::models::user> get_user_by_id(uuid id);

        shared<app::models::session> create_session(const std::string & ip, uint_least16_t port);

        void session_closed(const shared<app::models::session> & session, const char exception[]);

        void session_is_encrypted(const shared<app::models::session> & session);
        void session_is_upgrade(const shared<app::models::session> & session);
    };
}