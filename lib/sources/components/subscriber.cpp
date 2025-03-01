#include <boost/asio/co_spawn.hpp>
#include <boost/asio/consign.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/redis/connection.hpp>
#include <copper/components/cache.hpp>
#include <copper/components/subscriber.hpp>
#include <iostream>

namespace copper::components {
containers::async_of<void> subscriber() {
  boost::redis::request _request;

  _request.push("SUBSCRIBE", "Copper_Events");

  const auto _connection = co_await cache::instance()->get_connection();

  boost::redis::generic_response _response;
  _connection->set_receive_response(_response);

  while (_connection->will_reconnect()) {
    co_await _connection->async_exec(_request, boost::redis::ignore,
                                     boost::asio::deferred);

    for (boost::system::error_code _ec;;) {
      _connection->receive(_ec);
      if (_ec == boost::redis::error::sync_receive_push_failed) {
        _ec = {};

        co_await _connection->async_receive(boost::asio::deferred);
      }

      if (_ec) break;

      std::cout << _response.value().at(1).value << " "
                << _response.value().at(2).value << " "
                << _response.value().at(3).value << std::endl;

      boost::redis::consume_one(_response);
    }
  }
}
}  // namespace copper::components