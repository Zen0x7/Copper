//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/beast/http.hpp>
#include <boost/json/parse.hpp>
#include <boost/uuid/random_generator.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/database.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/invoke.hpp>
#include <copper/components/kernel.hpp>
#include <copper/components/shared.hpp>
#include <iostream>

namespace copper::components {
containers::async_of<kernel_call_result> invoke(std::string method,
                                                std::string signature,
                                                std::string headers,
                                                std::string body) {
  auto _generator = boost::uuids::random_generator();
  auto _configuration = configuration::instance();

  const auto _kernel = boost::make_shared<kernel>();

  auto _verb = boost::beast::http::string_to_verb(method);

  request _request_{_verb, signature, 11};
  _request_.set(fields::host, _configuration->get()->app_host_);
  _request_.set(fields::user_agent, "Copper");
  _request_.set(fields::content_type, "application/json");
  _request_.body() = body;
  _request_.prepare_payload();

  boost::system::error_code ec;
  boost::json::value _headers = boost::json::parse(headers, ec);

  if (ec) {
    std::cout << "Headers can't be parsed: " << ec.what() << std::endl;
  }

  if (!_headers.is_object()) {
    std::cout << "Headers must be an object" << std::endl;
  }

  for (auto& [key, value] : _headers.as_object()) {
    if (value.is_string()) {
      _request_.set(key, value.as_string());
    } else if (value.is_int64()) {
      _request_.set(key, std::to_string(value.as_int64()));
    } else if (value.is_uint64()) {
      _request_.set(key, std::to_string(value.as_uint64()));
    }
  }

  auto _request_id = _generator();
  auto _session_id = _generator();

  const std::string _ip = "127.0.0.1";
  auto _start_at = chronos::now();

  auto response = co_await _kernel->call(_session_id, "", std::move(_request_),
                                         _ip, _request_id, _start_at);

  co_return response;
}

containers::async_of<void> invoke_from_console(const std::string method,
                                               const std::string signature,
                                               const std::string headers,
                                               const std::string body) {
  auto [_request, _response, _message] =
      co_await invoke(method, signature, headers, body);

  std::cout << "Session: " << std::endl
            << "================" << std::endl
            << std::endl;
  std::cout << "ID: " << _request->session_id_ << std::endl << std::endl;

  std::cout << "Request: " << std::endl
            << "================" << std::endl
            << std::endl;

  std::cout << "ID: " << _request->id_ << std::endl;
  std::cout << "Headers: " << _request->headers_ << std::endl;
  std::cout << "Body: " << _request->body_ << std::endl << std::endl;

  std::cout << "Response: " << std::endl
            << "================" << std::endl
            << std::endl;

  std::cout << "ID: " << _response->id_ << std::endl;
  std::cout << "Status Code: " << _response->status_code_ << std::endl;
  std::cout << "Protected: " << (_response->protected_ ? "ON" : "OFF")
            << std::endl;
  std::cout << "Headers: " << _response->headers_ << std::endl;
  std::cout << "Body: " << _response->body_ << std::endl << std::endl;
}
}  // namespace copper::components
