#pragma once

#include <copper/components/shared.hpp>

#include <mutex>
#include <list>
#include <boost/asio.hpp>
#include <boost/scope/scope_exit.hpp>
#include <iostream>

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
        std::list<
                boost::asio::cancellation_signal
        > signals_;

    public:

        /**
         * Constructor
         * @param executor
         */
        explicit task_group(
                boost::asio::any_io_executor executor
        ) : timer_{
                std::move(executor),
                boost::asio::steady_timer::time_point::max()
        } {}

        /**
         * Overload Constructor
         */
        task_group(
                task_group const &
        ) = delete;

        /**
         * Overload Constructor
         */
        task_group(
                task_group &&
        ) = delete;

        /**
         * Adapt
         *
         * @tparam CompletionToken
         * @param completion_token
         * @return
         */
        template<
                typename CompletionToken
        >
        auto adapt(
                CompletionToken &&completion_token
        ) {
            auto guard = std::lock_guard{mutex_};
            auto signal = signals_.emplace(signals_.end());

            boost::weak_ptr<task_group> weak_self = shared_from_this();

            return boost::asio::bind_cancellation_slot(
                    signal->slot(),
                    boost::asio::consign(
                            std::forward<CompletionToken>(completion_token),
                            boost::scope::make_scope_exit(
                                    [weak_self, signal]() {

                                        if (auto self = weak_self.lock()) {
                                            auto guard = std::lock_guard{self->mutex_};

                                            self->signals_.erase(signal);

                                            if (self->signals_.empty()) self->timer_.cancel();
                                        }

                                    })));
        }

        /**
         * Emit cancellation signal
         *
         * @param type
         */
        void emit(
                boost::asio::cancellation_type type
        ) {
            auto guard = std::lock_guard{mutex_};
            for (auto &signal: signals_)
                signal.emit(type);
        }

        // LCOV_EXCL_START
        /**
         * Async wait
         *
         * @tparam CompletionToken
         * @param completion_token
         * @return
         */
        template<
                typename CompletionToken = boost::asio::default_completion_token_t<boost::asio::any_io_executor>
        >
        auto async_wait(
                CompletionToken &&completion_token = boost::asio::default_completion_token_t<boost::asio::any_io_executor>{}
        ) {
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

} // namespace copper::component