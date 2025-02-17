#pragma once

#include <copper/components/shared.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/dotenv.hpp>
#include <copper/components/report.hpp>
#include <copper/components/uuid.hpp>

#include <app/models/user.hpp>

#include <boost/mysql/connection_pool.hpp>
#include <boost/asio/thread_pool.hpp>

namespace copper::components {
    class database : public shared_enabled<database> {
        shared<boost::mysql::connection_pool> pool_;
        shared<boost::asio::thread_pool> thread_pool_;
    public:
        database();

        void start();

        containers::optional_of<app::models::user> get_user_by_email(const std::string &email);
    };
}