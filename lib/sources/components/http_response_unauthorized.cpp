#include <copper/components/chronos.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/dotenv.hpp>
#include <copper/components/http_fields.hpp>
#include <copper/components/http_response_unauthorized.hpp>
#include <copper/components/http_status_code.hpp>

namespace copper::components {

http_response http_response_unauthorized(const http_request &request,
                                         long start_at) {
  const auto now = chronos::now();

  http_response response{http_status_code::unauthorized, request.version()};
  response.set(http_fields::content_type, "application/json");

  response.set(http_fields::access_control_allow_headers,
               "Accept,Authorization,Content-Type,X-Requested-With");
  const auto allowed_origins = dotenv::getenv("HTTP_ALLOWED_ORIGINS", "*");
  response.set(http_fields::access_control_allow_origin, allowed_origins);

  response.set("X-Server", "Copper");
  response.set("X-Time", std::to_string(now - start_at));

  response.version(request.version());
  response.keep_alive(request.keep_alive());

  response.body() = std::string(R"({"message":"unauthorized"})");

  response.prepare_payload();

  return response;
}
}  // namespace copper::components