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

#include <copper/components/controllers/api/connections_controller.hpp>
#include <copper/components/core.hpp>

namespace copper::components::controllers::api {
void connections_controller::invoke(
    const shared<core>& core,
    const shared<controller_parameters>& parameters,
    const callback_of<res>& on_success,
    const callback_of<std::exception_ptr>& on_error) {
  json::array items;

  auto state_subscriptions = core->state_->get_subscriptions();
  const auto& index =
      state_subscriptions.get<subscriptions::by_connection_id>();

  for (const auto& [id, connection] : core->state_->get_connections()) {
    json::array subscriptions;
    const auto [current, next] = index.equal_range(id);

    // LCOV_EXCL_START
    for (auto it = current; it != next; ++it) {
      subscriptions.emplace_back(json::object{{"name", it->channel_}});
    }
    // LCOV_EXCL_STOP

    items.push_back(json::object{
        {"id", to_string(id)},
        {"subscriptions", subscriptions},
    });
  }

  const json::object data = {{"items", items}};

  on_success(make_response(core, parameters, status_code::ok, serialize(data),
                           "application/json"));
}
}  // namespace copper::components::controllers::api
