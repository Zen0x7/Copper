#include <boost/asio/as_tuple.hpp>
#include <boost/asio/cancel_after.hpp>
#include <copper/components/signal_handler.hpp>

namespace copper::components {

containers::async_of<void> signal_handler(
    const shared<task_group>& task_group) {
  const auto _executor = co_await boost::asio::this_coro::executor;
  auto _signal_set = boost::asio::signal_set{_executor, SIGINT, SIGTERM};

  auto _sig = co_await _signal_set.async_wait();

  if (_sig == SIGINT) {
    //    std::cout << "Gracefully cancelling child tasks...\n";
    task_group->emit(boost::asio::cancellation_type::total);

    auto [_ec] = co_await task_group->async_wait(boost::asio::as_tuple(
        boost::asio::cancel_after(std::chrono::seconds{10})));

    if (_ec == boost::asio::error::operation_aborted) {
      //      std::cout << "Sending a terminal cancellation signal...\n";
      task_group->emit(boost::asio::cancellation_type::terminal);
      co_await task_group->async_wait();
    }

    //    std::cout << "Child tasks completed.\n";
  } else  // SIGTERM
  {
    _executor.get_inner_executor().context().stop();
  }
}

}  // namespace copper::components