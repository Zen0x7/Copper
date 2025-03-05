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
    if (auto const should_break = _response->per_rule(value, _attribute, _rule);
        should_break)
      break;
  }

  _response->success_ = _response->errors_.empty();

  return _response;
  // LCOV_EXCL_START
}

bool validator::per_rule(const json::value &value, const std::string &attribute,
                         const std::string &rule) {
  if (attribute == "*") {
    if (!value.is_object()) {
      const std::string _error_message = "Message must be an JSON object.";
      this->insert_or_push(attribute, _error_message);
      return true;
    }
    return false;
  }

  containers::vector_of<std::string> _scoped_rules;
  split(_scoped_rules, rule, boost::is_any_of(","), boost::token_compress_off);

  for (const std::string &_scoped_rule : _scoped_rules) {
    std::cout << attribute << "|" << _scoped_rule << std::endl;

    if (const auto should_break =
            this->per_scope_rule(value, attribute, _scoped_rule);
        should_break) {
      break;
    }
  }
  return false;
}

bool validator::per_scope_rule(const json::value &value,
                               const std::string &attribute,
                               const std::string_view &rule) {
  if (!value.as_object().contains(attribute) && rule != "nullable") {
    const std::string _error_message =
        "Attribute " + attribute + " is required.";
    this->insert_or_push(attribute, _error_message);
    return true;
  }

  if (rule == "is_string") {
    this->on_string_rule(value, attribute);
  } else if (rule == "is_uuid") {
    this->on_uuid_rule(value, attribute);
  } else if (rule == "confirmed") {
    this->on_confirmation_rule(value, attribute);
  } else if (rule == "is_object") {
    this->on_object_rule(value, attribute);
  } else if (rule == "is_number") {
    this->on_number_rule(value, attribute);
  } else if (rule == "is_array_of_strings") {
    this->on_array_of_strings_rule(value, attribute);
  }

  return false;
}

void validator::on_confirmation_rule(const json::value &value,
                                     const std::string &attribute) {
  if (!value.as_object().contains(attribute + "_confirmation")) {
    const std::string _error_message =
        "Attribute " + attribute + "_confirmation" + " must be present.";
    this->insert_or_push(attribute, _error_message);
  } else {
    if (!value.as_object().at(attribute + "_confirmation").is_string()) {
      const std::string _error_message =
          "Attribute " + attribute + "_confirmation" + " must be string.";
      this->insert_or_push(attribute, _error_message);
    } else {
      const std::string value_{value.as_object().at(attribute).as_string()};
      const std::string value_confirmation_{
          value.as_object().at(attribute + "_confirmation").as_string()};
      if (value_ != value_confirmation_) {
        const std::string _error_message = "Attribute " + attribute + " and " +
                                           attribute + "_confirmation" +
                                           " must be equals.";
        this->insert_or_push(attribute, _error_message);
      }
    }
  }
}

void validator::on_array_of_strings_rule(const json::value &value,
                                         const std::string &attribute) {
  if (!value.as_object().at(attribute).is_array()) {
    const std::string _error_message =
        "Attribute " + attribute + " must be an array.";
    this->insert_or_push(attribute, _error_message);
  } else {
    this->on_array_of_strings_per_element_rule(value, attribute);
  }
}

void validator::on_array_of_strings_per_element_rule(
    const json::value &value, const std::string &attribute) {
  if (auto _elements = value.as_object().at(attribute).as_array();
      _elements.empty()) {
    const std::string _error_message =
        "Attribute " + attribute + " cannot be empty.";
    this->insert_or_push(attribute, _error_message);
  } else {
    size_t _i = 0;
    for (const auto &_element : _elements) {
      if (!_element.is_string()) {
        std::string _error_message = fmt::format(
            "Attribute {} at position {} must be string.", attribute, _i);
        this->insert_or_push(attribute, _error_message);
      }
      _i++;
    }
  }
}

void validator::on_number_rule(const json::value &value,
                               const std::string &attribute) {
  if (!value.as_object().at(attribute).is_int64()) {
    const std::string _error_message =
        "Attribute " + attribute + " must be a number.";
    this->insert_or_push(attribute, _error_message);
  }
}

void validator::on_object_rule(const json::value &value,
                               const std::string &attribute) {
  if (!value.as_object().at(attribute).is_object()) {
    const std::string _error_message =
        "Attribute " + attribute + " must be an object.";
    this->insert_or_push(attribute, _error_message);
  }
}

void validator::on_uuid_rule(const json::value &value,
                             const std::string &attribute) {
  if (!value.as_object().at(attribute).is_string()) {
    const std::string _error_message =
        "Attribute " + attribute + " must be string.";
    this->insert_or_push(attribute, _error_message);
  } else {
    try {
      boost::lexical_cast<boost::uuids::uuid>(
          value.as_object().at(attribute).as_string().data());
    } catch (boost::bad_lexical_cast & /* exception */) {
      const std::string _error_message =
          "Attribute " + attribute + " must be uuid.";
      this->insert_or_push(attribute, _error_message);
    }
  }
}

void validator::on_string_rule(const json::value &value,
                               const std::string &attribute) {
  if (!value.as_object().at(attribute).is_string()) {
    const std::string _error_message =
        "Attribute " + attribute + " must be string.";
    this->insert_or_push(attribute, _error_message);
  }
}
// LCOV_EXCL_STOP

}  // namespace copper::components