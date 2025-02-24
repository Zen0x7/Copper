#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/dotenv.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/gunzip.hpp>
#include <copper/components/response_cors.hpp>
#include <copper/components/method.hpp>
#include <copper/components/state.hpp>
#include <copper/components/status_code.hpp>

namespace copper::components {

response response_cors(const request &request, long start_at,
                       containers::vector_of<method> methods,
                       const shared<state> &state) {
  const auto now = chronos::now();

  response response{
      methods.empty() ? status_code::method_not_allowed : status_code::ok,
      request.version()};
  response.set(fields::content_type, "application/json");

  containers::vector_of<std::string> authorized_methods;
  for (auto &verb : methods) authorized_methods.push_back(to_string(verb));
  const auto methods_as_string = boost::join(authorized_methods, ",");
  response.set(fields::access_control_allow_methods,
               methods.empty() ? "" : methods_as_string);

  const std::string allowed_headers =
      "Accept,Authorization,Content-Type,X-Requested-With";

  response.set(fields::access_control_allow_headers, allowed_headers);
  const auto allowed_origins =
      state->get_configuration()->get()->http_allowed_origins_;

  response.set(fields::access_control_allow_origin, allowed_origins);

  response.set("X-Server", "Copper");
  response.set("X-Time", std::to_string(now - start_at));

  response.version(request.version());
  response.keep_alive(request.keep_alive());

  if (!request["Accept-Encoding"].empty() &&
      boost::contains(request["Accept-Encoding"], "gzip")) {
    response.body() = gunzip_compress("{}");
    response.set(fields::content_encoding, "gzip");
  } else {
    response.body() = "{}";
  }

  response.prepare_payload();

  return response;
}
}  // namespace copper::components