#pragma once

#include <mutex>
#include <list>
#include <boost/asio.hpp>
#include <boost/scope/scope_exit.hpp>

namespace copper::components {
    class task_group {
        std::mutex mutex_;
        boost::asio::steady_timer timer_;
        std::list<boost::asio::cancellation_signal> signals_;

    public:
        task_group(boost::asio::any_io_executor executor)
                : timer_{std::move(executor), boost::asio::steady_timer::time_point::max()} {}

        task_group(task_group const &) = delete;

        task_group(task_group &&) = delete;

        template<typename CompletionToken>
        auto
        adapt(CompletionToken &&completion_token) {
            auto guard = std::lock_guard{mutex_};
            auto signal = signals_.emplace(signals_.end());

            return boost::asio::bind_cancellation_slot(
                    signal->slot(),
                    boost::asio::consign(
                            std::forward<CompletionToken>(completion_token),
                            boost::scope::make_scope_exit(
                                    [this, signal]() {
                                        auto guard = std::lock_guard{mutex_};
                                        if (signals_.erase(signal) == signals_.end())
                                            timer_.cancel();
                                    })));
        }

        void
        emit(boost::asio::cancellation_type type) {
            auto guard = std::lock_guard{mutex_};
            for (auto &signal: signals_)
                signal.emit(type);
        }

        // LCOV_EXCL_START
        template<typename CompletionToken = boost::asio::default_completion_token_t<boost::asio::any_io_executor>>
        auto
        async_wait(CompletionToken &&completion_token = boost::asio::default_completion_token_t<boost::asio::any_io_executor>{}) {
            return boost::asio::async_compose<CompletionToken, void(boost::system::error_code)>(
                    [this, scheduled = false](
                            auto &&self, boost::system::error_code ec = {}) mutable {
                        if (!scheduled)
                            self.reset_cancellation_state(
                                    boost::asio::enable_total_cancellation());

                        if (!self.cancelled() && ec == boost::asio::error::operation_aborted)
                            ec = {};

                        {
                            auto lg = std::lock_guard{mutex_};

                            if (!signals_.empty() && !ec) {
                                scheduled = true;
                                return timer_.async_wait(std::move(self));
                            }
                        }

                        if (!std::exchange(scheduled, true))
                            return boost::asio::post(boost::asio::append(std::move(self), ec));

                        self.complete(ec);
                    },
                    completion_token,
                    timer_);
        }
        // LCOV_EXCL_STOP
    };
}