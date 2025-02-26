#include <boost/algorithm/string/predicate.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/gunzip.hpp>
#include <copper/components/response_not_found.hpp>
#include <copper/components/state.hpp>
#include <copper/components/status_code.hpp>
#include <copper/components/views.hpp>

namespace copper::components {

response response_not_found(const request &request, const long start_at,
                            const shared<state> &state) {
  response _response{status_code::not_found, request.version()};

  const bool _requires_html =
      request.count(fields::accept) > 0 &&
      boost::contains(request.at(fields::accept), "html");

  const auto _allowed_origins =
      state->get_configuration()->get()->http_allowed_origins_;

  _response.set(fields::access_control_allow_origin, _allowed_origins);

  _response.version(request.version());
  _response.keep_alive(request.keep_alive());
  if (_requires_html) {
    _response.set(fields::content_type, "text/html");
  } else {
    _response.set(fields::content_type, "application/json");
  }

  if (!request["Accept-Encoding"].empty() &&
      boost::contains(request["Accept-Encoding"], "gzip")) {
    _response.set(fields::content_encoding, "gzip");
    if (_requires_html) {
      _response.body() = gunzip_compress(state->get_views()->render("404"));
    } else {
      _response.body() = gunzip_compress("{}");
    }
  } else {
    if (_requires_html) {
      _response.body() = state->get_views()->render("404");
    } else {
      _response.body() = "{}";
    }
  }

  _response.prepare_payload();

  const auto _now = chronos::now();
  _response.set("X-Server", "Copper");
  _response.set("X-Time", std::to_string(_now - start_at));

  return _response;
}
}  // namespace copper::components