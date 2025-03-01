#include <boost/algorithm/string/predicate.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/gunzip.hpp>
#include <copper/components/response_too_many_requests.hpp>
#include <copper/components/state.hpp>
#include <copper/components/status_code.hpp>

namespace copper::components {

response response_too_many_requests(const request &request, const long start_at,
                                    const int ttl) {
  response _response{status_code::too_many_requests, request.version()};

  const auto _allowed_origins =
      configuration::instance()->get()->http_allowed_origins_;

  _response.set(fields::access_control_allow_origin, _allowed_origins);

  _response.version(request.version());
  _response.keep_alive(request.keep_alive());
  _response.set(fields::content_type, "application/json");

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
  _response.set("X-Rate-Until", std::to_string(ttl));

  return _response;
}
}  // namespace copper::components