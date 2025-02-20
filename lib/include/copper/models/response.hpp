#pragma once

#include <boost/core/span.hpp>
#include <boost/describe/class.hpp>
#include <boost/mysql/datetime.hpp>
#include <boost/optional.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <copper/components/http_header.hpp>
#include <copper/components/http_request.hpp>
#include <copper/components/http_response.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>
#include <copper/models/request.hpp>
#include <copper/models/session.hpp>
#include <cstdint>

namespace copper::models {

class response : public copper::components::shared_enabled<response> {
 public:
  std::string id_;
  std::string session_id_;
  std::string request_id_;
  int status_code_;
  std::string headers_;
  std::string body_;

  response(std::string id, std::string session_id, std::string request_id,
           int status_code, std::string headers, std::string body)
      : id_(id),
        session_id_(session_id),
        request_id_(request_id),
        status_code_(status_code),
        headers_(headers),
        body_(body) {}
};

copper::components::shared<response> response_from_http_response(
    copper::components::uuid session,
    const copper::components::shared<request> &request,
    const copper::components::http_response &http_response);
}  // namespace copper::models