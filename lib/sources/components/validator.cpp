#include <copper/components/validator.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/uuid.hpp>

namespace copper::components {

    void validator::insert_or_push(
            const std::string &key,
            const std::string &message
    ) {
        if (!this->errors.contains(key)) this->errors[key] = boost::json::array({});

        this->errors.at(key).as_array().emplace_back(message);
    }

    boost::shared_ptr<
            validator
    > validator_make(
            const std::map<
                    std::string,
                    std::string
            > &rules,
            const boost::json::value &value
    ) {
        auto response = boost::make_shared<validator>();

        for (auto &rule: rules) {
            if (rule.first == "*") {
                if (!value.is_object()) {
                    const std::string error_message = "Message must be an JSON object.";
                    response->insert_or_push(rule.first, error_message);
                    break;
                }
            } else {
                if (!value.as_object().contains(rule.first)) {
                    std::string error_message = "Attribute " + rule.first + " is required.";
                    response->insert_or_push(rule.first, error_message);
                } else {
                    std::vector<std::string> scoped_rules;

                    size_t start = 0;
                    size_t end = rule.second.find(",");

                    while (end != std::string::npos) {
                        scoped_rules.push_back(rule.second.substr(start, end - start));
                        start = end + 1;
                        end = rule.second.find(",", start);
                    }
                    scoped_rules.push_back(rule.second.substr(start));

                    for (auto &scoped_rule: scoped_rules) {
                        if (scoped_rule == "is_string") {
                            if (!value.as_object().at(rule.first).is_string()) {
                                std::string error_message = "Attribute " + rule.first + " must be string.";
                                response->insert_or_push(rule.first, error_message);
                            }
                        } else if (scoped_rule == "is_uuid") {
                            if (!value.as_object().at(rule.first).is_string()) {
                                std::string error_message = "Attribute " + rule.first + " must be string.";
                                response->insert_or_push(rule.first, error_message);
                            } else {
                                try {
                                    boost::lexical_cast<boost::uuids::uuid>(
                                            value.as_object().at(rule.first).as_string().data());
                                } catch (boost::bad_lexical_cast &exp) {
                                    std::string error_message = "Attribute " + rule.first + " must be uuid.";
                                    response->insert_or_push(rule.first, error_message);
                                }
                            }
                        } else if (scoped_rule == "confirmed") {
                            if (!value.as_object().contains(rule.first + "_confirmation")) {
                                std::string error_message
                                        = "Attribute " + rule.first + "_confirmation" + " must be present.";
                                response->insert_or_push(rule.first, error_message);
                            } else {
                                if (!value.as_object().at(rule.first + "_confirmation").is_string()) {
                                    std::string error_message
                                            = "Attribute " + rule.first + "_confirmation" + " must be string.";
                                    response->insert_or_push(rule.first, error_message);
                                } else {
                                    const std::string value_{value.as_object().at(rule.first).as_string()};
                                    const std::string value_confirmation_{
                                            value.as_object().at(rule.first + "_confirmation").as_string()};
                                    if (value_ != value_confirmation_) {
                                        std::string error_message = "Attribute " + rule.first + " and " + rule.first
                                                                    + "_confirmation" + " must be equals.";
                                        response->insert_or_push(rule.first, error_message);
                                    }
                                }
                            }
                        } else if (scoped_rule == "is_object") {
                            if (!value.as_object().at(rule.first).is_object()) {
                                std::string error_message = "Attribute " + rule.first + " must be an object.";
                                response->insert_or_push(rule.first, error_message);
                            }
                        } else if (scoped_rule == "is_number") {
                            if (!value.as_object().at(rule.first).is_int64()) {
                                std::string error_message = "Attribute " + rule.first + " must be a number.";
                                response->insert_or_push(rule.first, error_message);
                            }
                        } else if (scoped_rule == "is_array_of_strings") {
                            if (!value.as_object().at(rule.first).is_array()) {
                                std::string error_message = "Attribute " + rule.first + " must be an array.";
                                response->insert_or_push(rule.first, error_message);
                            } else {
                                if (auto elements = value.as_object().at(rule.first).as_array(); elements.empty()) {
                                    std::string error_message = "Attribute " + rule.first + " cannot be empty.";
                                    response->insert_or_push(rule.first, error_message);
                                } else {
                                    size_t i = 0;
                                    for (const auto &element: elements) {
                                        if (!element.is_string()) {
                                            std::string error_message = "Attribute " + rule.first + " at position "
                                                                        + std::to_string(i) + " must be string.";
                                            response->insert_or_push(rule.first, error_message);
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

        response->success = response->errors.empty();

        return response; }

}