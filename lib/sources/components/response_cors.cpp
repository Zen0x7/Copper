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
                       const containers::vector_of<method> &methods) {
  response _response{
      methods.empty() ? status_code::method_not_allowed : status_code::ok,
      request.version()};

  containers::vector_of<std::string> _authorized_methods;
  for (const auto &_verb : methods)
    _authorized_methods.push_back(to_string(_verb));
  const auto _methods_as_string = boost::join(_authorized_methods, ",");
  _response.set(fields::access_control_allow_methods,
                methods.empty() ? "" : _methods_as_string);

  const std::string _allowed_headers = "Accept,Authorization,Content-Type";
  _response.set(fields::access_control_allow_headers, _allowed_headers);
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

  return _response;
}
}  // namespace copper::components