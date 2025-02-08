#pragma once

#include <boost/json.hpp>
#include <boost/smart_ptr.hpp>
#include <map>
#include <string>

namespace copper::components::validator {

    class validator_response : public std::enable_shared_from_this<validator_response> {
    public:
        boost::json::object errors;
        bool success = false;

        void insert_or_push(const std::string &key, const std::string &message);

        validator_response() {}
    };

    boost::shared_ptr<validator_response> make(const std::map<std::string, std::string> &rules, const boost::json::value &value);

}  // namespace copper::components::validator

