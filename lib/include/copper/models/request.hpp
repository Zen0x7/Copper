#ifndef COPPER_MODELS_REQUEST_HPP
#define COPPER_MODELS_REQUEST_HPP

#pragma once

#include <boost/core/span.hpp>
#include <boost/describe/class.hpp>
#include <boost/mysql/datetime.hpp>
#include <boost/optional.hpp>
#include <copper/components/http_header.hpp>
#include <copper/components/http_path.hpp>
#include <copper/components/http_query.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>
#include <cstdint>

namespace copper::models {

/**
 * Request
 */
class request : public components::shared_enabled<request> {
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
   * Version
   */
  std::string version_;

  /**
   * Method
   */
  std::string method_;

  /**
   * Path
   */
  std::string path_;

  /**
   * Query
   */
  std::string query_;

  /**
   * Headers
   */
  std::string headers_;

  /**
   * Body
   */
  std::string body_;

  /**
   * Started at
   */
  long started_at_;

  /**
   * Finished at
   */
  long finished_at_;

  /**
   * Duration
   */
  long duration_;

  /**
   * Constructor
   *
   * @param id
   * @param session_id
   * @param version
   * @param method
   * @param path
   * @param query
   * @param headers
   * @param body
   * @param started_at
   * @param finished_at
   * @param duration
   */
  request(std::string id, std::string session_id, std::string version,
          std::string method, std::string path, std::string query,
          std::string headers, std::string body, long started_at,
          long finished_at, long duration)
      : id_(id),
        session_id_(session_id),
        version_(version),
        method_(method),
        path_(path),
        query_(query),
        headers_(headers),
        body_(body),
        started_at_(started_at),
        finished_at_(finished_at),
        duration_(duration) {}
};

/**
 * Create request from HTTP request
 *
 * @param session_id
 * @param request_id
 * @param now
 * @param request
 * @return
 */
components::shared<request> request_from_http_request(
    components::uuid session_id, components::uuid request_id, long now,
    const components::http_request &request);

}  // namespace copper::models

#endif