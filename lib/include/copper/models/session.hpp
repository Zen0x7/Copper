#pragma once

#include <boost/core/span.hpp>
#include <boost/describe/class.hpp>
#include <boost/mysql/datetime.hpp>
#include <boost/optional.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>
#include <cstdint>

namespace copper::models {
class session : public components::shared_enabled<session> {
 public:
  std::string id_;
  std::string ip_;
  uint_least16_t port_;
  long started_at_;
  long finished_at_;

  session(std::string id, std::string ip, uint_least16_t port, long started_at,
          long finished_at)
      : id_(id),
        ip_(ip),
        port_(port),
        started_at_(started_at),
        finished_at_(finished_at) {}
};
}  // namespace copper::models