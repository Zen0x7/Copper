#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/json.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/validator.hpp>

namespace copper::components {

void validator::insert_or_push(const std::string &key,
                               const std::string &message) {
  if (!this->errors_.contains(key)) this->errors_[key] = boost::json::array({});

  this->errors_.at(key).as_array().emplace_back(message);
}

shared<validator> validator_make(const containers::map_of_strings &rules,
                                 const json::value &value) {
  auto _response = boost::make_shared<validator>();

  for (auto &_rule : rules) {
    if (_rule.first == "*") {
      if (!value.is_object()) {
        const std::string _error_message = "Message must be an JSON object.";
        _response->insert_or_push(_rule.first, _error_message);
        break;
      }
    } else {
      if (!value.as_object().contains(_rule.first)) {
        std::string _error_message =
            "Attribute " + _rule.first + " is required.";
        _response->insert_or_push(_rule.first, _error_message);
      } else {
        containers::vector_of<std::string> scoped_rules;

        size_t start = 0;
        size_t end = _rule.second.find(",");

        while (end != std::string::npos) {
          scoped_rules.push_back(_rule.second.substr(start, end - start));
          start = end + 1;
          end = _rule.second.find(",", start);
        }
        scoped_rules.push_back(_rule.second.substr(start));

        for (auto &scoped_rule : scoped_rules) {
          if (scoped_rule == "is_string") {
            if (!value.as_object().at(_rule.first).is_string()) {
              std::string _error_message =
                  "Attribute " + _rule.first + " must be string.";
              _response->insert_or_push(_rule.first, _error_message);
            }
          } else if (scoped_rule == "is_uuid") {
            if (!value.as_object().at(_rule.first).is_string()) {
              std::string _error_message =
                  "Attribute " + _rule.first + " must be string.";
              _response->insert_or_push(_rule.first, _error_message);
            } else {
              try {
                boost::lexical_cast<boost::uuids::uuid>(
                    value.as_object().at(_rule.first).as_string().data());
              } catch (boost::bad_lexical_cast &exp) {
                std::string _error_message =
                    "Attribute " + _rule.first + " must be uuid.";
                _response->insert_or_push(_rule.first, _error_message);
              }
            }
          } else if (scoped_rule == "confirmed") {
            if (!value.as_object().contains(_rule.first + "_confirmation")) {
              std::string _error_message = "Attribute " + _rule.first +
                                           "_confirmation" +
                                           " must be present.";
              _response->insert_or_push(_rule.first, _error_message);
            } else {
              if (!value.as_object()
                       .at(_rule.first + "_confirmation")
                       .is_string()) {
                std::string _error_message = "Attribute " + _rule.first +
                                             "_confirmation" +
                                             " must be string.";
                _response->insert_or_push(_rule.first, _error_message);
              } else {
                const std::string value_{
                    value.as_object().at(_rule.first).as_string()};
                const std::string value_confirmation_{
                    value.as_object()
                        .at(_rule.first + "_confirmation")
                        .as_string()};
                if (value_ != value_confirmation_) {
                  std::string _error_message =
                      "Attribute " + _rule.first + " and " + _rule.first +
                      "_confirmation" + " must be equals.";
                  _response->insert_or_push(_rule.first, _error_message);
                }
              }
            }
          } else if (scoped_rule == "is_object") {
            if (!value.as_object().at(_rule.first).is_object()) {
              std::string _error_message =
                  "Attribute " + _rule.first + " must be an object.";
              _response->insert_or_push(_rule.first, _error_message);
            }
          } else if (scoped_rule == "is_number") {
            if (!value.as_object().at(_rule.first).is_int64()) {
              std::string _error_message =
                  "Attribute " + _rule.first + " must be a number.";
              _response->insert_or_push(_rule.first, _error_message);
            }
          } else if (scoped_rule == "is_array_of_strings") {
            if (!value.as_object().at(_rule.first).is_array()) {
              std::string _error_message =
                  "Attribute " + _rule.first + " must be an array.";
              _response->insert_or_push(_rule.first, _error_message);
            } else {
              if (auto elements = value.as_object().at(_rule.first).as_array();
                  elements.empty()) {
                std::string _error_message =
                    "Attribute " + _rule.first + " cannot be empty.";
                _response->insert_or_push(_rule.first, _error_message);
              } else {
                size_t i = 0;
                for (const auto &element : elements) {
                  if (!element.is_string()) {
                    std::string _error_message =
                        "Attribute " + _rule.first + " at position " +
                        std::to_string(i) + " must be string.";
                    _response->insert_or_push(_rule.first, _error_message);
                  }
                  i++;
                }
              }
            }
          }
        }
      }
    }
  }

  _response->success_ = _response->errors_.empty();

  return _response;
  // LCOV_EXCL_START
}
// LCOV_EXCL_STOP

}  // namespace copper::components