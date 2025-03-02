//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/gunzip.hpp>
#include <copper/components/method.hpp>
#include <copper/components/response_cors.hpp>
#include <copper/components/response_shared_handler.hpp>
#include <copper/components/status_code.hpp>

namespace copper::components {

response response_cors(const request &request, const long start_at,
                       const containers::vector_of<method> &methods) {
  response _response{
      methods.empty() ? status_code::method_not_allowed : status_code::ok,
      request.version()};

  containers::vector_of<std::string> _authorized_methods;
  for (const auto &_verb : methods)
    _authorized_methods.push_back(to_string(_verb));
  const auto _methods_as_string = boost::join(_authorized_methods, ",");
  _response.set(fields::access_control_allow_methods,
                methods.empty() ? "" : _methods_as_string);

  const std::string _allowed_headers = "Accept,Authorization,Content-Type";
  _response.set(fields::access_control_allow_headers, _allowed_headers);

  return response_shared_handler(request, _response, start_at);
}
}  // namespace copper::components