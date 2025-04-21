#ifndef COPPER_COMPONENTS_VALIDATOR_HPP
#define COPPER_COMPONENTS_VALIDATOR_HPP

#pragma once

// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <fmt/format.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <copper/components/containers.hpp>
#include <copper/components/json.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {

/**
 * Validator
 */
class validator final : public shared_enabled<validator> {
 public:
  /**
   * Errors
   */
  json::object errors_;

  /**
   * Status
   */
  bool success_ = false;

  /**
   * Insert or push message to errors
   *
   * @param key
   * @param message
   */
  void insert_or_push(const std::string& key, const std::string& message) {
    if (!this->errors_.contains(key))
      this->errors_[key] = boost::json::array({});

    this->errors_.at(key).as_array().emplace_back(message);
  }

  /**
   * Constructor
   */
  validator() = default;

  /**
   * Per rule
   *
   * @param value
   * @param attribute
   * @param rule
   */
  bool per_rule(const json::value& value,
                const std::string& attribute,
                const std::string& rule) {
    if (attribute == "*") {
      if (!value.is_object()) {
        const std::string _error_message = "Message must be an JSON object.";
        this->insert_or_push(attribute, _error_message);
        return true;
      }
      return false;
    }

    containers::vector_of<std::string> _scoped_rules;
    split(_scoped_rules, rule, boost::is_any_of(","),
          boost::token_compress_off);

    for (const std::string& _scoped_rule : _scoped_rules) {
      if (const auto should_break =
              this->per_scope_rule(value, attribute, _scoped_rule);
          should_break) {
        break;
      }
    }
    return false;
  }

  /**
   * Per scope rule
   *
   * @param value
   * @param attribute
   * @param rule
   * @return
   */
  bool per_scope_rule(const json::value& value,
                      const std::string& attribute,
                      const std::string_view& rule) {
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

  /**
   * On confirmation rule
   *
   * @param value
   * @param attribute
   */
  void on_confirmation_rule(const json::value& value,
                            const std::string& attribute) {
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
          const std::string _error_message =
              "Attribute " + attribute + " and " + attribute + "_confirmation" +
              " must be equals.";
          this->insert_or_push(attribute, _error_message);
        }
      }
    }
  }

  /**
   * On array of strings rule
   * @param value
   * @param attribute
   */
  void on_array_of_strings_rule(const json::value& value,
                                const std::string& attribute) {
    if (!value.as_object().at(attribute).is_array()) {
      const std::string _error_message =
          "Attribute " + attribute + " must be an array.";
      this->insert_or_push(attribute, _error_message);
    } else {
      this->on_array_of_strings_per_element_rule(value, attribute);
    }
  }

  /**
   * On array of strings per element rule
   * @param value
   * @param attribute
   */
  void on_array_of_strings_per_element_rule(const json::value& value,
                                            const std::string& attribute) {
    if (auto _elements = value.as_object().at(attribute).as_array();
        _elements.empty()) {
      const std::string _error_message =
          "Attribute " + attribute + " cannot be empty.";
      this->insert_or_push(attribute, _error_message);
    } else {
      size_t _i = 0;
      for (const auto& _element : _elements) {
        if (!_element.is_string()) {
          std::string _error_message = fmt::format(
              "Attribute {} at position {} must be string.", attribute, _i);
          this->insert_or_push(attribute, _error_message);
        }
        _i++;
      }
    }
  }

  /**
   * On number rule
   *
   * @param value
   * @param attribute
   */
  void on_number_rule(const json::value& value, const std::string& attribute) {
    if (!value.as_object().at(attribute).is_int64()) {
      const std::string _error_message =
          "Attribute " + attribute + " must be a number.";
      this->insert_or_push(attribute, _error_message);
    }
  }

  /**
   * On object rule
   *
   * @param value
   * @param attribute
   */
  void on_object_rule(const json::value& value, const std::string& attribute) {
    if (!value.as_object().at(attribute).is_object()) {
      const std::string _error_message =
          "Attribute " + attribute + " must be an object.";
      this->insert_or_push(attribute, _error_message);
    }
  }

  /**
   * On uuid rule
   *
   * @param value
   * @param attribute
   */
  void on_uuid_rule(const json::value& value, const std::string& attribute) {
    if (!value.as_object().at(attribute).is_string()) {
      const std::string _error_message =
          "Attribute " + attribute + " must be string.";
      this->insert_or_push(attribute, _error_message);
    } else {
      try {
        boost::lexical_cast<boost::uuids::uuid>(
            value.as_object().at(attribute).as_string().data());
      } catch (boost::bad_lexical_cast& /* exception */) {
        const std::string _error_message =
            "Attribute " + attribute + " must be uuid.";
        this->insert_or_push(attribute, _error_message);
      }
    }
  };

  /**
   * On string rule
   *
   * @param value
   * @param attribute
   */
  void on_string_rule(const json::value& value, const std::string& attribute) {
    if (!value.as_object().at(attribute).is_string()) {
      const std::string _error_message =
          "Attribute " + attribute + " must be string.";
      this->insert_or_push(attribute, _error_message);
    }
  }
};

/**
 * Validator factory
 *
 * @param rules
 * @param value
 * @return boost::shared_ptr<validator> validator
 */
inline shared<validator> validator_make(const containers::map_of_strings& rules,
                                        const json::value& value) {
  auto _response = boost::make_shared<validator>();

  for (const auto& [_attribute, _rule] : rules) {
    if (auto const should_break = _response->per_rule(value, _attribute, _rule);
        should_break)
      break;
  }

  _response->success_ = _response->errors_.empty();

  return _response;
}

}  // namespace copper::components

#endif
