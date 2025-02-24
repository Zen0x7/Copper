#include <boost/algorithm/string/predicate.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/dotenv.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/gunzip.hpp>
#include <copper/components/response_not_found.hpp>
#include <copper/components/state.hpp>
#include <copper/components/status_code.hpp>
#include <copper/components/views.hpp>

namespace copper::components {

response response_not_found(const request &request,
                            long start_at,
                            const shared<state> &state) {
  const auto now = chronos::now();

  response response{status_code::not_found, request.version()};

  bool requires_html = request.count(fields::accept) > 0 &&
                       boost::contains(request.at(fields::accept), "html");
  if (requires_html) {
    response.set(fields::content_type, "text/html");
  } else {
    response.set(fields::content_type, "application/json");
  }

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
    response.set(fields::content_encoding, "gzip");
    if (requires_html) {
      response.body() = gunzip_compress(state->get_views()->render("404"));
    } else {
      response.body() = gunzip_compress("{}");
    }
  } else {
    if (requires_html) {
      response.body() = state->get_views()->render("404");
    } else {
      response.body() = "{}";
    }
  }

  response.prepare_payload();

  return response;
}
}  // namespace copper::components