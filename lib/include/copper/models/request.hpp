#ifndef COPPER_MODELS_REQUEST_HPP
#define COPPER_MODELS_REQUEST_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/header.hpp>
#include <copper/components/query.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>
#include <utility>

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
      : id_(std::move(id)),
        session_id_(std::move(session_id)),
        version_(std::move(version)),
        method_(std::move(method)),
        path_(std::move(path)),
        query_(std::move(query)),
        headers_(std::move(headers)),
        body_(std::move(body)),
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
components::shared<request> request_from_request(
    components::uuid session_id, components::uuid request_id, long now,
    const components::request &request);

}  // namespace copper::models

#endif