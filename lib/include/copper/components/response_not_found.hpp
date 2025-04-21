#ifndef COPPER_COMPONENTS_RESPONSE_NOT_FOUND_HPP
#define COPPER_COMPONENTS_RESPONSE_NOT_FOUND_HPP

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
#include <copper/components/response_shared_handler.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/status_code.hpp>

namespace copper::components {
class core;

/**
 * Generate response on not found
 *
 * @param core
 * @param request
 * @param start_at
 * @return res
 */
inline res res_not_found(const shared<core>& core,
                         const req& request,
                         long start_at) {
  res _response{status_code::not_found, request.version()};

  return res_shared_handler(core, request, _response, start_at);
}

}  // namespace copper::components

#endif
