#include <boost/algorithm/string/predicate.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/dotenv.hpp>
#include <copper/components/http_fields.hpp>
#include <copper/components/http_response_not_found.hpp>
#include <copper/components/http_status_code.hpp>
#include <copper/components/state.hpp>
#include <copper/components/views.hpp>

namespace copper::components {

http_response http_response_not_found(const http_request &request,
                                      long start_at,
                                      const shared<state> &state) {
  const auto now = chronos::now();

  http_response response{http_status_code::not_found, request.version()};

  bool requires_html = request.count(http_fields::accept) > 0 &&
                       boost::contains(request.at(http_fields::accept), "html");
  if (requires_html) {
    response.set(http_fields::content_type, "text/html");
  } else {
    response.set(http_fields::content_type, "application/json");
  }

  const std::string allowed_headers =
      "Accept,Authorization,Content-Type,X-Requested-With";

  response.set(http_fields::access_control_allow_headers, allowed_headers);
  const auto allowed_origins =
      state->get_configuration()->get()->http_allowed_origins_;

  response.set(http_fields::access_control_allow_origin, allowed_origins);

  response.set("X-Server", "Copper");
  response.set("X-Time", std::to_string(now - start_at));

  response.version(request.version());
  response.keep_alive(request.keep_alive());

  if (requires_html) {
    response.body() = state->get_views()->render("404");
  } else {
    response.body() = std::string(R"({"message":"not_found"})");
  }

  response.prepare_payload();

  return response;
}
}  // namespace copper::components