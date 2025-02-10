#include <copper/components/signal_handler.hpp>

namespace copper::components {
    boost::asio::awaitable<
            void,
            boost::asio::strand<
                    boost::asio::io_context::executor_type
            >
    > signal_handler(task_group &task_group) {
        auto executor = co_await boost::asio::this_coro::executor;
        auto signal_set = boost::asio::signal_set{executor, SIGINT, SIGTERM};

        auto sig = co_await signal_set.async_wait();

        if (sig == SIGINT) {
            std::cout << "Gracefully cancelling child tasks...\n";
            task_group.emit(boost::asio::cancellation_type::total);

            // Wait a limited time for child tasks to gracefully cancell
            auto [ec] = co_await task_group.async_wait(
                    boost::asio::as_tuple(boost::asio::cancel_after(std::chrono::seconds{10})));

            if (ec == boost::asio::error::operation_aborted) // Timeout occurred
            {
                std::cout << "Sending a terminal cancellation signal...\n";
                task_group.emit(boost::asio::cancellation_type::terminal);
                co_await task_group.async_wait();
            }

            std::cout << "Child tasks completed.\n";
        } else // SIGTERM
        {
            executor.get_inner_executor().context().stop();
        }
    }
}