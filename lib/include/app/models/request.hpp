#pragma once

#include <cstdint>
#include <boost/core/span.hpp>
#include <boost/describe/class.hpp>
#include <boost/optional.hpp>
#include <boost/mysql/datetime.hpp>

#include <copper/components/uuid.hpp>
#include <copper/components/shared.hpp>

namespace app::models {
    using namespace copper::components;

    class request : public shared_enabled<request> {
    public:
      std::string id_;
      std::string session_id_;
      std::string version_;
      std::string method_;
      std::string path_;
      std::string headers_;
      std::string body_;
      long started_at_;
      long finished_at_;
      long duration_;

      request(
        std::string id,
        std::string session_id,
        std::string version,
        std::string method,
        std::string path,
        std::string headers,
        std::string body,
        long started_at,
        long finished_at,
        long duration
        ) :
        id_(id),
        session_id_(session_id),
        version_(version),
        method_(method),
        path_(path),
        headers_(headers),
        body_(body),
        started_at_(started_at),
        finished_at_(finished_at),
        duration_(duration)
      {}
    };
}