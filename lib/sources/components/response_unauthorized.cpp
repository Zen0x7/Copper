#include <boost/algorithm/string/predicate.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/gunzip.hpp>
#include <copper/components/response_unauthorized.hpp>
#include <copper/components/state.hpp>
#include <copper/components/status_code.hpp>

namespace copper::components {

response response_unauthorized(const request &request, const long start_at,
                               const shared<state> &state) {
  response _response{status_code::unauthorized, request.version()};
  _response.set(fields::content_type, "application/json");

  const std::string _allowed_headers =
      "Accept,Authorization,Content-Type,X-Requested-With";

  _response.set(fields::access_control_allow_headers, _allowed_headers);
  const auto _allowed_origins =
      state->get_configuration()->get()->http_allowed_origins_;

  _response.set(fields::access_control_allow_origin, _allowed_origins);

  _response.version(request.version());
  _response.keep_alive(request.keep_alive());

  if (!request["Accept-Encoding"].empty() &&
      boost::contains(request["Accept-Encoding"], "gzip")) {
    _response.body() = gunzip_compress("{}");
    _response.set(fields::content_encoding, "gzip");
  } else {
    _response.body() = "{}";
  }

  _response.prepare_payload();

  const auto _now = chronos::now();
  _response.set("X-Server", "Copper");
  _response.set("X-Time", std::to_string(_now - start_at));

  return _response;
}
}  // namespace copper::components