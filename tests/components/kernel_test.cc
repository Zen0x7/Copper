//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <copper/components/database.hpp>
#include <copper/components/event.hpp>
#include <copper/components/json.hpp>
#include <copper/components/kernel.hpp>
#include <copper/components/router.hpp>
#include <copper/components/uuid.hpp>
#include <copper/controllers/api/auth_controller.hpp>
#include <copper/controllers/api/up_controller.hpp>
#include <copper/controllers/api/user_controller.hpp>
#include <future>

copper::components::containers::async_of<void> cancel_context() {
  const auto executor = co_await boost::asio::this_coro::executor;
  executor.get_inner_executor().context().stop();
}

TEST(Components_Kernel, Handle) {
  using namespace copper::components;

  auto _kernel = boost::make_shared<kernel>();
  constexpr auto _threads = 16;
  boost::asio::io_context _io_context{_threads};

  database::setup(_io_context);

  router::instance()->push(
      method::get, "/api/is_alive",
      boost::make_shared<copper::controllers::api::up_controller>(),
      {.use_throttler_ = true, .use_protector_ = false, .rpm_ = 100});

  const uuid _session_id = boost::uuids::random_generator()();
  const uuid _websocket_id = boost::uuids::random_generator()();

  {
    co_spawn(
        make_strand(_io_context),
        [&]() -> containers::async_of<void> {
          const std::string _message = "<html></html>";
          const auto _event =
              co_await _kernel->handle(_session_id, _websocket_id, _message);

          std::cout << "On [" << _message << "] kernel respond: ["
                    << _event->data_ << "]" << std::endl;

          assert(_event->status_code_ == status_code::unprocessable_entity);
          const auto _data = serialize(_event->data_);
          assert(boost::contains(_data, "command"));
          assert(boost::contains(_data, "ack"));
          assert(boost::contains(_data, "message"));
          assert(boost::contains(_data, "status"));
          assert(boost::contains(_data, "The given data was an invalid JSON."));
          assert(boost::contains(_data, "422"));
        },
        boost::asio::detached);
  }

  {
    co_spawn(
        make_strand(_io_context),
        [&]() -> containers::async_of<void> {
          const std::string _message = "{}";
          const auto _event =
              co_await _kernel->handle(_session_id, _websocket_id, _message);

          std::cout << "On [" << _message << "] kernel respond: ["
                    << _event->data_ << "]" << std::endl;

          assert(_event->status_code_ == status_code::unprocessable_entity);
          const auto _data = serialize(_event->data_);
          assert(boost::contains(_data, "command"));
          assert(boost::contains(_data, "message"));
          assert(boost::contains(_data, "errors"));
          assert(boost::contains(_data, "status"));
          assert(boost::contains(_data, "The given data was invalid."));
          assert(boost::contains(_data, "Attribute command is required."));
          assert(boost::contains(_data, "Attribute id is required."));
          assert(boost::contains(_data, "422"));
        },
        boost::asio::detached);
  }

  {
    co_spawn(
        make_strand(_io_context),
        [&]() -> containers::async_of<void> {
          const std::string _message = serialize(boost::json::object({
              {"id", to_string(boost::uuids::random_generator()())},
              {"command", "not_found"},
          }));
          const auto _event =
              co_await _kernel->handle(_session_id, _websocket_id, _message);

          std::cout << "On [" << _message << "] kernel respond: ["
                    << _event->data_ << "]" << std::endl;

          assert(_event->status_code_ == status_code::not_found);
          const auto _data = serialize(_event->data_);
          assert(boost::contains(_data, "command"));
          assert(boost::contains(_data, "message"));
          assert(boost::contains(_data, "status"));
          assert(
              boost::contains(_data, "The requested command doesn't exists."));
          assert(boost::contains(_data, "404"));
        },
        boost::asio::detached);
  }

  {
    co_spawn(
        make_strand(_io_context),
        [&_kernel, &_session_id,
         &_websocket_id]() -> containers::async_of<void> {
          const std::string _message = serialize(boost::json::object({
              {"id", to_string(boost::uuids::random_generator()())},
              {"command", "invoke"},
              {"method", "GET"},
              {"signature", "/api/is_alive"},
              {"headers", "{}"},
              {"body", "{}"},
          }));
          const auto _event =
              co_await _kernel->handle(_session_id, _websocket_id, _message);
          std::cout << "On [" << _message << "] kernel respond: ["
                    << _event->data_ << "]" << std::endl;
          assert(_event->status_code_ == status_code::ok);
          const auto _data = serialize(_event->data_);
          assert(boost::contains(_data, "command"));
          assert(boost::contains(_data, "data"));
          assert(boost::contains(_data, "headers"));
          assert(boost::contains(_data, "body"));
          assert(boost::contains(_data, "200"));
          co_await cancel_context();
        },
        boost::asio::detached);
  }

  _io_context.run();

  ASSERT_TRUE(true);

  database::instance_.reset();
}
