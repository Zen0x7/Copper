#include <copper/components/http_kernel.hpp>

#include <copper/components/http_response_too_many_requests.hpp>
#include <copper/components/http_response_unauthorized.hpp>
#include <copper/components/http_response_exception.hpp>
#include <copper/components/http_response_cors.hpp>
#include <copper/components/http_response_bad_request.hpp>
#include <copper/components/http_response_not_found.hpp>

#include <copper/components/http_controller.hpp>

#include <copper/components/authentication.hpp>
#include <copper/components/validator.hpp>

#include <copper/components/http_route_match.hpp>
#include <copper/components/http_route_find.hpp>

#include <copper/components/state.hpp>

#include <copper/components/mime_type.hpp>

#include <app/models/request.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>

namespace copper::components {

    containers::optional_of<http_kernel_result> http_kernel::find_on_routes(const http_request &request) const {
      for (const auto & [route, controller] : *state_->get_http_router()->get_routes()) {
        if (auto [matches, bindings] = http_route_match(request.method(), request.target(), route); matches) {
          return http_kernel_result {
            .route_ = route,
            .controller_ = controller,
            .bindings_ = bindings
          };
        }
      }

      return boost::none;
    }

    containers::vector_of<http_method> http_kernel::get_available_methods(const http_request &request) const {
      containers::vector_of<http_method> methods;
      for (const auto& [route, controller] : *state_->get_http_router()->get_routes()) {
        if (auto [matches, bindings] = http_route_find(request.target(), route); matches)
          methods.push_back(route.method_);
      }
      return methods; }

    boost::asio::awaitable<
      std::pair<shared<app::models::request>, http_response_generic>,
      boost::asio::strand<
        boost::asio::io_context::executor_type
      >
    >
    http_kernel::invoke(
      const shared<app::models::session> & session,
      boost::beast::string_view,
      const http_request &request,
      const std::string &ip,
      const uuid & request_id,
      long now
      ) const {

      json::object headers;
      int headers_count = 0;
      for (const auto& header : request.base()) {
        headers[header.name_string()] = header.value();
        headers_count++;
      }

      const size_t query_ask_symbol_position = request.target().find('?');
      const bool path_has_parameters = query_ask_symbol_position != std::string::npos;
      const std::string path {
        path_has_parameters ? request.target().substr(0, query_ask_symbol_position) : request.target()
      };
      const std::string query {
        path_has_parameters ? request.target().substr(query_ask_symbol_position) : ""
      };

      auto _request = boost::make_shared<app::models::request>(
        to_string(request_id),
        session->id_,
        std::to_string(request.version()),
        std::string(request.method_string()),
        path,
        query,
        headers_count > 0 ? serialize(headers) : "",
        std::string(request.body()),
        now,
        0,
        0
      );

      if (const auto route = find_on_routes(request); route.has_value()) {
        containers::unordered_map_of_strings bindings = route.value().bindings_;

        route.value().controller_->set_start(now);

        if (route.value().controller_->config_.use_throttler) {
          if (auto [can, TTL] = co_await state_->get_cache()->can_invoke(request, ip,
                                                                         route.value().controller_->config_.rpm); !can) {
            co_return std::make_pair(_request, http_response_too_many_requests(request, now, TTL));
          }
        }

        if (route.value().controller_->config_.use_auth) {
          std::string bearer {request["Authorization"]};

          std::string token = boost::starts_with(bearer, "Bearer ") ? bearer.substr(7) : bearer;

          boost::optional<authentication_result> user_id;
          user_id = authentication_from_bearer(bearer, dotenv::getenv("APP_KEY"));

          if (!user_id.has_value()) co_return std::make_pair(_request, http_response_unauthorized(request, now));

          route.value().controller_->set_user(user_id.get().id);
        }

        route.value().controller_->set_bindings(bindings);
        route.value().controller_->set_state(state_);

        if (route.value().controller_->config_.use_validator) {
          boost::system::error_code json_parse_error_code;
          boost::json::value value = boost::json::parse(request.body(), json_parse_error_code);

          if (!json_parse_error_code) {
            route.value().controller_->set_data(value);
            auto rules = route.value().controller_->rules();

            if (auto validator = validator_make(rules, value); !validator->success) {
              auto error_response = boost::json::object(
                {{"message", "The given data was invalid."}, {"errors", validator->errors}});

              co_return std::make_pair(_request, route.value().controller_->response(
                request, http_status_code::unprocessable_entity,
                serialize(error_response), "application/json"));
            }
          } else {
            auto error_response
              = boost::json::object({{"message", "The given data was invalid."},
                                     {"errors", {{"*", "The body must be a valid JSON."}}}});

            co_return std::make_pair(_request, route.value().controller_->response(
              request, http_status_code::unprocessable_entity, serialize(error_response),
              "application/json"));
          }
        }

        try {
          co_return std::make_pair(_request, route.value().controller_->invoke(request));
        } catch (std::exception& exception) {
          std::cout << exception.what() << std::endl;

          co_return std::make_pair(_request, http_response_exception(request, now));
        }
      }

      if (request.method() == http_method::options) {
        auto available_verbs = get_available_methods(request);
        co_return std::make_pair(_request, http_response_cors(request, now, available_verbs));
      }

      if (http_request_is_illegal(request)) {
        co_return std::make_pair(_request, http_response_bad_request(request, now));
      }

      co_return std::make_pair(_request, http_response_not_found(request, now));
    }
}