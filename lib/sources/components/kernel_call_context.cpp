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

#include <copper/components/kernel_call_context.hpp>

#include <copper/components/logger.hpp>
#include <copper/components/response.hpp>
#include <copper/components/response_bad_request.hpp>
#include <copper/components/response_cors.hpp>
#include <copper/components/response_not_found.hpp>
#include <copper/components/response_too_many_requests.hpp>
#include <copper/components/response_unauthorized.hpp>
#include <copper/components/url.hpp>

namespace copper::components {
kernel_call_context::kernel_call_context(const shared<core>& core,
                                         const uuid server_id,
                                         const shared<connection>& connection,
                                         const req& request,
                                         long start_at,
                                         callback_of<res> callback)
    : core_(core),
      server_id_(server_id),
      connection_(connection),
      request_(request),
      start_at_(start_at),
      callback_(std::move(callback)),
      url_(url_from_request(request)) {}

void kernel_call_context::start() {
  LOG("[kernel_call_context@start] scope_in");

  if (request_is_illegal(request_)) {
    LOG("[kernel_call_context@start] invalid request");
    const auto res = res_bad_request(core_, request_, start_at_);
    return finalize(res);
  }

  if (request_.method() == method::options) {
    LOG("[kernel_call_context@start] CORS OPTIONS detected");
    const auto verbs = kernel::get_available_methods(core_, url_);
    const auto res = res_cors(core_, request_, start_at_, verbs);
    return finalize(res);
  }

  route_ = kernel::find_on_routes(core_, request_.method(), url_);
  if (!route_) {
    LOG("[kernel_call_context@start] scope_out [1 of 2]");
    return return_not_found();
  }

  ++route_->controller_->count_;
  bindings_ = route_->bindings_;

  check_throttle();
  LOG("[kernel_call_context@start] scope_out [2 of 2]");
}

void kernel_call_context::check_throttle() {
  LOG("[kernel_call_context@check_throttle] scope_in");
  if (!route_->controller_->controller_configuration_.use_throttler_) {
    LOG("[kernel_call_context@check_throttle] scope_out [1 of 2]");
    return check_auth();
  }

  core_->cache_->can_invoke(
      request_, connection_->ip_,
      route_->controller_->controller_configuration_.rpm_,
      [self = shared_from_this()](bool can, int TTL) {
        LOG("[kernel_call_context@start] [cache@can_invoke] scope_in");
        if (!can) {
          LOG("[kernel_call_context@start] [cache@can_invoke] rejected");
          auto res = res_too_many_requests(self->core_, self->request_,
                                           self->start_at_, TTL);
          self->finalize(res);
        } else {
          LOG("[kernel_call_context@start] [cache@can_invoke] allowed");
          self->check_auth();
        }
        LOG("[kernel_call_context@start] [cache@can_invoke] scope_out");
      });
  LOG("[kernel_call_context@check_throttle] scope_out [2 of 2]");
}

void kernel_call_context::check_auth() {
  LOG("[kernel_call_context@check_auth] scope_in");
  if (!route_->controller_->controller_configuration_.use_auth_) {
    LOG("[kernel_call_context@check_auth] scope_out [1 of 3]");
    return check_validation();
  }

  std::string bearer = request_["Authorization"];
  std::string token =
      boost::starts_with(bearer, "Bearer ") ? bearer.substr(7) : bearer;

  user_ =
      authentication_from_bearer(token, core_->configuration_->get()->app_key_);

  if (!user_.has_value()) {
    auto res = res_unauthorized(core_, request_, start_at_);

    LOG("[kernel_call_context@check_auth] scope_out [2 of 3]");
    return finalize(res, /*is_protected=*/true);
  }

  check_validation();
  LOG("[kernel_call_context@check_auth] scope_out [3 of 3]");
}

void kernel_call_context::check_validation() {
  LOG("[kernel_call_context@check_validation] scope_in");
  if (!route_->controller_->controller_configuration_.use_validator_) {
    LOG("[kernel_call_context@check_validation] scope_out [1 of 4]");
    return invoke_controller();
  }

  boost::system::error_code ec;
  body_ = boost::json::parse(request_.body(), ec);
  // LCOV_EXCL_START
  if (ec) {
    const auto err =
        json::object({{"message", "The given data was invalid."},
                      {"errors", {{"*", "The body must be a valid JSON."}}}});
    const auto res = route_->controller_->make_response(
        core_, build_parameters(), status_code::unprocessable_entity,
        serialize(err), "application/json");
    LOG("[kernel_call_context@check_validation] scope_out [2 of 4]");
    return finalize(res);
  }
  // LCOV_EXCL_STOP

  auto validator = validator_make(route_->controller_->rules(), body_);

  // LCOV_EXCL_START
  if (!validator->success_) {
    const auto err = json::object({{"message", "The given data was invalid."},
                                   {"errors", validator->errors_}});
    const auto res = route_->controller_->make_response(
        core_, build_parameters(), status_code::unprocessable_entity,
        serialize(err), "application/json");
    LOG("[kernel_call_context@check_validation] scope_out [3 of 4]");
    return finalize(res);
  }
  // LCOV_EXCL_STOP

  invoke_controller();
  LOG("[kernel_call_context@check_validation] scope_out [4 of 4]");
}

void kernel_call_context::invoke_controller() {
  LOG("[kernel_call_context@invoke_controller] scope_in");
  const auto params = build_parameters();
  route_->controller_->invoke(
      core_, params,
      [self = shared_from_this()](const res& res) {
        LOG("[kernel_call_context@invoke_controller] "
            "[any_controller@invoke] [on_success] scope_in");
        self->finalize(res, self->route_->controller_->controller_configuration_
                                .use_protector_);
        LOG("[kernel_call_context@invoke_controller] "
            "[any_controller@invoke] [on_success] scope_out");
      },
      [self = shared_from_this()](std::exception_ptr eptr) {
        LOG("[kernel_call_context@invoke_controller] "
            "[any_controller@invoke] [on_error] scope_in");
        try {
          if (eptr)
            std::rethrow_exception(eptr);
        } catch (const std::exception& e) {
          LOG("Kernel Controller Error: " << e.what());
          auto res =
              res_exception(self->core_, self->request_, self->start_at_);
          self->finalize(res, self->route_->controller_
                                  ->controller_configuration_.use_protector_);
        }
        LOG("[kernel_call_context@invoke_controller] [any_controller@invoke] "
            "[on_error] scope_out");
      });
  LOG("[kernel_call_context@invoke_controller] scope_out");
}

shared<controller_parameters> kernel_call_context::build_parameters() {
  return boost::make_shared<controller_parameters>(request_, body_, user_,
                                                   bindings_, start_at_);
}

void kernel_call_context::finalize(const res& res, bool is_protected) const {
  LOG("[kernel_call_context@finalize] scope_in");
#ifdef DEBUG_ENABLED
  auto req_log = request_from_req(connection_, start_at_, request_);
  auto resp_log = response_factory(connection_, req_log, res);
  resp_log->protected_ = is_protected;
  req_log->finished_at_ = chronos::now();
  req_log->duration_ = req_log->finished_at_ - start_at_;

  LOG("Finalize: " << res.body());
  core_->database_->create_invocation(core_, req_log, resp_log);
#endif

  callback_(res);
  LOG("[kernel_call_context@finalize] scope_out");
}

void kernel_call_context::return_not_found() const {
  LOG("[kernel_call_context@return_not_found] scope_in");
  const auto res = res_not_found(core_, request_, start_at_);
  finalize(res);
  LOG("[kernel_call_context@return_not_found] scope_out");
}
}  // namespace copper::components
