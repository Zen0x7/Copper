#ifndef COPPER_MODELS_RESPONSE_HPP
#define COPPER_MODELS_RESPONSE_HPP

#pragma once

#include <boost/core/span.hpp>
#include <boost/describe/class.hpp>
#include <boost/mysql/datetime.hpp>
#include <boost/optional.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <copper/components/header.hpp>
#include <copper/components/request.hpp>
#include <copper/components/response.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>
#include <copper/models/request.hpp>
#include <copper/models/session.hpp>
#include <cstdint>
#include <utility>

namespace copper::models {

/**
 * Response
 */
class response : public components::shared_enabled<response> {
 public:
  /**
   * ID
   */
  std::string id_;

  /**
   * Session ID
   */
  std::string session_id_;

  /**
   * Request ID
   */
  std::string request_id_;

  /**
   * Status code
   */
  int status_code_;

  /**
   * Headers
   */
  std::string headers_;

  /**
   * Body
   */
  std::string body_;

  /**
   * Protected
   */
  bool protected_;

  /**
   * Constructor
   *
   * @param id
   * @param session_id
   * @param request_id
   * @param status_code
   * @param headers
   * @param body
   */
  response(std::string id, std::string session_id, std::string request_id,
           int status_code, std::string headers, std::string body)
      : id_(std::move(id)),
        session_id_(std::move(session_id)),
        request_id_(std::move(request_id)),
        status_code_(status_code),
        headers_(std::move(headers)),
        body_(std::move(body)),
        protected_(false) {}
};

/**
 * Create response from HTTP response
 *
 * @param session_id
 * @param request
 * @param service_response
 * @return
 */
components::shared<response> response_from_response(
    components::uuid session_id, const components::shared<request> &request,
    const components::response &service_response);

}  // namespace copper::models

#endif