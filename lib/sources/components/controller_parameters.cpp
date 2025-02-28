#include <copper/components/controller_parameters.hpp>

namespace copper::components {
controller_parameters::controller_parameters(
    const request &request, const json::value &body,
    const containers::optional_of<authentication_result> &auth,
    const containers::unordered_map_of_strings &bindings, const long &start_at)
    : request_(request),
      body_(body),
      auth_(auth),
      bindings_(bindings),
      start_at_(start_at) {}

const request &controller_parameters::get_request() const { return request_; }

const json::value &controller_parameters::get_body() const { return body_; }

const containers::optional_of<authentication_result> &
controller_parameters::get_auth() const {
  return auth_;
}

const containers::unordered_map_of_strings &
controller_parameters::get_bindings() const {
  return bindings_;
}

const long &controller_parameters::get_start_at() const { return start_at_; }
}  // namespace copper::components
