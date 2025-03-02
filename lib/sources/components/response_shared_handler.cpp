#include <boost/algorithm/string/predicate.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/gunzip.hpp>
#include <copper/components/response_shared_handler.hpp>

namespace copper::components {
response response_shared_handler(const request &request, response &response,
                                 const long start_at, const int ttl) {
  const bool _requires_html =
      request.count(fields::accept) > 0 &&
      boost::contains(request.at(fields::accept), "html");

  const auto _allowed_origins =
      configuration::instance()->get()->http_allowed_origins_;

  response.set(fields::access_control_allow_origin, _allowed_origins);

  response.version(request.version());
  response.keep_alive(request.keep_alive());

  if (_requires_html) {
    response.set(fields::content_type, "text/html");
  } else {
    response.set(fields::content_type, "application/json");
  }

  if (!request["Accept-Encoding"].empty() &&
      boost::contains(request["Accept-Encoding"], "gzip")) {
    response.set(fields::content_encoding, "gzip");
    if (_requires_html) {
      response.body() = gunzip_compress(
          views::instance()->render(std::to_string(response.result_int())));
    } else {
      response.body() = gunzip_compress("{}");
    }
  } else {
    if (_requires_html) {
      response.body() =
          views::instance()->render(std::to_string(response.result_int()));
    } else {
      response.body() = "{}";
    }
  }

  response.prepare_payload();

  const auto _now = chronos::now();
  response.set("X-Server", "Copper");
  response.set("X-Time", std::to_string(_now - start_at));

  if (ttl != -1) {
    response.set("X-Rate-Until", std::to_string(ttl));
  }

  return response;
}
}  // namespace copper::components
