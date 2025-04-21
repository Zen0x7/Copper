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

#include <copper/components/authentication.hpp>
#include <copper/components/cache.hpp>
#include <copper/components/core.hpp>
#include <copper/components/kernel.hpp>
#include <copper/components/kernel_call_context.hpp>
#include <copper/components/request.hpp>
#include <copper/components/validator.hpp>
#include <utility>

namespace copper::components {

void kernel::call(const shared<core>& core,
                  const uuid server_id,
                  const shared<connection>& connection,
                  const req& req,
                  long start_at,
                  callback_of<boost::beast::http::message_generator> callback) {
  const auto ctx = boost::make_shared<kernel_call_context>(
      core, server_id, connection, req, start_at, std::move(callback));
  LOG("[kernel@call] scope_in");
  ctx->start();
  LOG("[kernel@call] scope_out");
}
}  // namespace copper::components
