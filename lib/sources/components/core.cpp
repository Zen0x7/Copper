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

#include <copper/components/core.hpp>
#include <copper/components/state.hpp>

namespace copper::components {
core::core(const shared<cache>& cache,
           const shared<state>& state,
           const shared<logger>& logger,
           const shared<database>& database,
           const shared<configuration>& configuration,
           const shared<router>& router,
           const shared<views>& views)
    : cache_(cache),
      state_(state),
      logger_(logger),
      database_(database),
      configuration_(configuration),
      router_(router),
      views_(views) {}

shared<core> core::factory(boost::asio::io_context& ioc) {
  return boost::make_shared<core>(cache::factory(ioc), state::factory(),
                                  logger::factory(), database::factory(),
                                  configuration::factory(), router::factory(),
                                  views::factory());
}
}  // namespace copper::components
