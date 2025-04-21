// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <copper/components/state.hpp>
#include <set>

namespace copper::components {

state::state() = default;

shared<state> state::factory() {
  return boost::make_shared<state>();
}

void state::connected(shared<connection>& connection) {
  std::lock_guard lock(mutex_);
  connections_.try_emplace(connection->id_, connection);
}

void state::disconnected(const uuid id) {
  std::lock_guard lock(mutex_);
  connections_.erase(id);
}

containers::uuid_hash_map_of<weaker<connection>> state::get_connections()
    const {
  return connections_;
}

subscriptions::container state::get_subscriptions() const {
  return subscriptions_;
}

void state::subscribe(const uuid& id, const std::string& channel) {
  std::lock_guard lock(mutex_);
  subscriptions_.insert(subscription(id, channel));
}

void state::unsubscribe(const uuid& id, const std::string& channel) {
  auto& _index = subscriptions_.get<subscriptions::by_connection_id>();
  auto [_begin, _end] = _index.equal_range(id);

  for (auto _it = _begin; _it != _end;) {
    if (_it->channel_ == channel)
      _it = _index.erase(_it);
    // LCOV_EXCL_START
    else
      ++_it;
    // LCOV_EXCL_STOP
  }
}

bool state::is_subscribed(const uuid& id, const std::string& channel) {
  const auto& _index = subscriptions_.get<subscriptions::by_connection_id>();
  const auto [_current, _next] = _index.equal_range(id);
  for (auto _it = _current; _it != _next; ++_it) {
    if (_it->channel_ == channel && _it->connection_id_ == id) {
      return true;
    }
  }
  return false;
}

void state::broadcast(const containers::vector_of<std::string>& channels,
                      const std::string& message) {
  const auto& _index = subscriptions_.get<subscriptions::by_channel_name>();
  std::set<uuid> _receivers;

  for (auto& _channel : channels) {
    for (auto _it = _index.find(_channel); _it != _index.end(); ++_it) {
      if (_it->channel_ == _channel) {
        _receivers.insert(_it->connection_id_);  // Doesn't requires
                                                 // checks. See function
                                                 // description ...
      }
    }
  }

  {
    std::lock_guard lock(mutex_);
    for (auto& _id : _receivers) {
      if (connections_.contains(_id)) {
        if (const auto sp = connections_[_id].lock()) {
          sp->send(boost::make_shared<std::string const>(message));
        }
      }
    }
  }
}

void state::broadcast_all(const std::string& message) {
  {
    std::lock_guard lock(mutex_);
    for (auto& [_id, _connection] : connections_) {
      if (const auto sp = _connection.lock()) {
        sp->send(boost::make_shared<std::string const>(message));
      }
    }
  }
}

}  // namespace copper::components
