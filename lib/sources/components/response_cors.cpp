#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/gunzip.hpp>
#include <copper/components/method.hpp>
#include <copper/components/response_cors.hpp>
#include <copper/components/state.hpp>
#include <copper/components/status_code.hpp>

namespace copper::components {

response response_cors(const request &request, const long start_at,
                       const containers::vector_of<method>& methods,
                       const shared<state> &state) {
  const auto _now = chronos::now();

  response _response{
      methods.empty() ? status_code::method_not_allowed : status_code::ok,
      request.version()};
  _response.set(fields::content_type, "application/json");

  containers::vector_of<std::string> authorized_methods;
  for (const auto &_verb : methods)
    authorized_methods.push_back(to_string(_verb));
  const auto _methods_as_string = boost::join(authorized_methods, ",");
  _response.set(fields::access_control_allow_methods,
                methods.empty() ? "" : _methods_as_string);

  const std::string _allowed_headers =
      "Accept,Authorization,Content-Type,X-Requested-With";

  _response.set(fields::access_control_allow_headers, _allowed_headers);
  const auto _allowed_origins =
      state->get_configuration()->get()->http_allowed_origins_;

  _response.set(fields::access_control_allow_origin, _allowed_origins);

  _response.set("X-Server", "Copper");
  _response.set("X-Time", std::to_string(_now - start_at));

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

  return _response;
}
}  // namespace copper::components