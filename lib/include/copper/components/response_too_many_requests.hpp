#ifndef COPPER_COMPONENTS_RESPONSE_TOO_MANY_REQUESTS_HPP
#define COPPER_COMPONENTS_RESPONSE_TOO_MANY_REQUESTS_HPP

#pragma once

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

#include <copper/components/req.hpp>
#include <copper/components/res.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {
class core;

/**
 * Generate response on rate limit exceeded
 *
 * @param core
 * @param request
 * @param start_at
 * @param ttl
 * @return res
 */
inline res res_too_many_requests(const shared<core>& core,
                                 const req& request,
                                 long start_at,
                                 int ttl) {
  res _response{status_code::too_many_requests, request.version()};
  return res_shared_handler(core, request, _response, start_at, ttl);
}

}  // namespace copper::components

#endif
