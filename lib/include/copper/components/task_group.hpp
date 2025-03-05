#ifndef COPPER_COMPONENTS_TASK_GROUP_HPP
#define COPPER_COMPONENTS_TASK_GROUP_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/asio/append.hpp>
#include <boost/asio/bind_cancellation_slot.hpp>
#include <boost/asio/compose.hpp>
#include <boost/asio/consign.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/scope/scope_exit.hpp>
#include <copper/components/shared.hpp>
#include <list>
#include <mutex>

namespace copper::components {

class task_group : public shared_enabled<task_group> {
  /**
   * Mutex
   */
  std::mutex mutex_;

  /**
   * Timer
   */
  boost::asio::steady_timer timer_;

  /**
   * Signals
   */
  std::list<boost::asio::cancellation_signal> signals_;

 public:
  /**
   * Constructor
   * @param executor
   */
  explicit task_group(const boost::asio::any_io_executor &executor)
      : timer_{executor, boost::asio::steady_timer::time_point::max()} {}

  /**
   * Overload Constructor
   */
  task_group(task_group const &) = delete;

  /**
   * Overload Constructor
   */
  task_group(task_group &&) = delete;

  /**
   * Adapt
   *
   * @tparam CompletionToken
   * @param completion_token
   * @return
   */
  template <typename CompletionToken>
  auto adapt(CompletionToken &&completion_token) {
    auto _guard = std::scoped_lock{mutex_};
    auto _signal = signals_.emplace(signals_.end());

    boost::weak_ptr<task_group> _weak_self = shared_from_this();

    return boost::asio::bind_cancellation_slot(
        _signal->slot(),
        boost::asio::consign(
            std::forward<CompletionToken>(completion_token),
            boost::scope::make_scope_exit([_weak_self, _signal]() {
              if (const auto _self = _weak_self.lock()) {
                auto _local_guard = std::lock_guard{_self->mutex_};

                _self->signals_.erase(_signal);

                if (_self->signals_.empty()) _self->timer_.cancel();
              }
            })));
  }

  /**
   * Emit cancellation signal
   *
   * @param type
   */
  void emit(boost::asio::cancellation_type type);

  // LCOV_EXCL_START
  /**
   * Async wait
   *
   * @tparam CompletionToken
   * @param completion_token
   * @return
   */
  template <typename CompletionToken = boost::asio::default_completion_token_t<
                boost::asio::any_io_executor> >
  auto async_wait(CompletionToken &&completion_token = // NOSONAR
                      boost::asio::default_completion_token_t< // NOSONAR
                          boost::asio::any_io_executor>{}) { // NOSONAR
    return boost::asio::async_compose<CompletionToken,
                                      void(boost::system::error_code)>(
        [this, scheduled = false](auto &&self, // NOSONAR
                                  boost::system::error_code ec = {}) mutable {
          if (!scheduled)
            self.reset_cancellation_state(
                boost::asio::enable_total_cancellation());

          if (!self.cancelled() && ec == boost::asio::error::operation_aborted)
            ec = {};

          {
            auto lg = std::lock_guard{mutex_};

            if (!signals_.empty() && !ec) {
              scheduled = true;
              return timer_.async_wait(std::move(self)); // NOSONAR
            }
          }

          if (!std::exchange(scheduled, true))
            return boost::asio::post(boost::asio::append(std::move(self), ec)); // NOSONAR

          self.complete(ec);
        },
        completion_token, timer_);
  }
  // LCOV_EXCL_STOP
};

}  // namespace copper::components

#endif