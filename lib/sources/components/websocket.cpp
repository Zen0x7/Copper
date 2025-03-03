#include <boost/uuid/random_generator.hpp>
#include <copper/components/state.hpp>
#include <copper/components/uuid.hpp>
#include <copper/components/websocket.hpp>

namespace copper::components {
websocket::websocket(const uuid session_id,
                     boost::asio::ip::tcp::socket &&socket)
    : id_(boost::uuids::random_generator()()),
      session_id_(session_id),
      stream_(std::move(socket)) {}

websocket::~websocket() { state::instance()->disconnected(this->id_); }
}  // namespace copper::components
