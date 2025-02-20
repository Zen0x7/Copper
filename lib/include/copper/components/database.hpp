#pragma once

#include <copper/components/shared.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/dotenv.hpp>
#include <copper/components/report.hpp>
#include <copper/components/uuid.hpp>

#include <copper/models/user.hpp>
#include <copper/models/session.hpp>
#include <copper/models/request.hpp>
#include <copper/models/response.hpp>

#include <boost/mysql/connection_pool.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/strand.hpp>

namespace copper::components {
    class database : public shared_enabled<database> {
        shared<boost::mysql::connection_pool> pool_;
    public:
        database(const shared<boost::mysql::connection_pool> & pool);

        void start();

        containers::optional_of<shared<copper::models::user>> get_user_by_email(const std::string &email);

        shared<copper::models::user> get_user_by_id(uuid id);

        shared<copper::models::session> create_session(const std::string & ip, uint_least16_t port);

        void session_closed(const shared<copper::models::session> & session, const char exception[]);

        void session_is_encrypted(const shared<copper::models::session> & session);
        void session_is_upgrade(const shared<copper::models::session> & session);

      boost::asio::awaitable<
        void,
        boost::asio::strand<
          boost::asio::io_context::executor_type
        >
      > create_request(shared<copper::models::request> request, shared<copper::models::response> response);
    };
}