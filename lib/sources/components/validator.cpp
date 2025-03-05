//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <fmt/format.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/json.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/validator.hpp>
#include <iostream>

namespace copper::components {
void validator::insert_or_push(const std::string &key,
                               const std::string &message) {
  if (!this->errors_.contains(key)) this->errors_[key] = boost::json::array({});

  this->errors_.at(key).as_array().emplace_back(message);
}

shared<validator> validator_make(const containers::map_of_strings &rules,
                                 const json::value &value) {
  auto _response = boost::make_shared<validator>();

  for (const auto &[_attribute, _rule] : rules) {
    if (_attribute == "*") {
      if (!value.is_object()) {
        const std::string _error_message = "Message must be an JSON object.";
        _response->insert_or_push(_attribute, _error_message);
        break;
      }
    } else {
      containers::vector_of<std::string> _scoped_rules;
      split(_scoped_rules, _rule, boost::is_any_of(","),
            boost::token_compress_off);

      for (const std::string &_scoped_rule : _scoped_rules) {
        if (!value.as_object().contains(_attribute) &&
            _scoped_rule != "nullable") {
          std::string _error_message =
              "Attribute " + _attribute + " is required.";
          _response->insert_or_push(_attribute, _error_message);
          break;
        }

        if (_scoped_rule == "is_string") {
          if (!value.as_object().at(_attribute).is_string()) {
            std::string _error_message =
                "Attribute " + _attribute + " must be string.";
            _response->insert_or_push(_attribute, _error_message);
          }
        } else if (_scoped_rule == "is_uuid") {
          if (!value.as_object().at(_attribute).is_string()) {
            std::string _error_message =
                "Attribute " + _attribute + " must be string.";
            _response->insert_or_push(_attribute, _error_message);
          } else {
            try {
              boost::lexical_cast<boost::uuids::uuid>(
                  value.as_object().at(_attribute).as_string().data());
            } catch (boost::bad_lexical_cast & /* exception */) {
              std::string _error_message =
                  "Attribute " + _attribute + " must be uuid.";
              _response->insert_or_push(_attribute, _error_message);
            }
          }
        } else if (_scoped_rule == "confirmed") {
          if (!value.as_object().contains(_attribute + "_confirmation")) {
            std::string _error_message = "Attribute " + _attribute +
                                         "_confirmation" + " must be present.";
            _response->insert_or_push(_attribute, _error_message);
          } else {
            if (!value.as_object()
                     .at(_attribute + "_confirmation")
                     .is_string()) {
              std::string _error_message = "Attribute " + _attribute +
                                           "_confirmation" + " must be string.";
              _response->insert_or_push(_attribute, _error_message);
            } else {
              const std::string value_{
                  value.as_object().at(_attribute).as_string()};
              const std::string value_confirmation_{
                  value.as_object()
                      .at(_attribute + "_confirmation")
                      .as_string()};
              if (value_ != value_confirmation_) {
                std::string _error_message =
                    "Attribute " + _attribute + " and " + _attribute +
                    "_confirmation" + " must be equals.";
                _response->insert_or_push(_attribute, _error_message);
              }
            }
          }
        } else if (_scoped_rule == "is_object") {
          if (!value.as_object().at(_attribute).is_object()) {
            std::string _error_message =
                "Attribute " + _attribute + " must be an object.";
            _response->insert_or_push(_attribute, _error_message);
          }
        } else if (_scoped_rule == "is_number") {
          if (!value.as_object().at(_attribute).is_int64()) {
            std::string _error_message =
                "Attribute " + _attribute + " must be a number.";
            _response->insert_or_push(_attribute, _error_message);
          }
        } else if (_scoped_rule == "is_array_of_strings") {
          if (!value.as_object().at(_attribute).is_array()) {
            std::string _error_message =
                "Attribute " + _attribute + " must be an array.";
            _response->insert_or_push(_attribute, _error_message);
          } else {
            if (auto _elements = value.as_object().at(_attribute).as_array();
                _elements.empty()) {
              std::string _error_message =
                  "Attribute " + _attribute + " cannot be empty.";
              _response->insert_or_push(_attribute, _error_message);
            } else {
              size_t _i = 0;
              for (const auto &_element : _elements) {
                if (!_element.is_string()) {
                  std::string _error_message =
                      fmt::format("Attribute {} at position {} must be string.",
                                  _attribute, _i);
                  _response->insert_or_push(_attribute, _error_message);
                }
                _i++;
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