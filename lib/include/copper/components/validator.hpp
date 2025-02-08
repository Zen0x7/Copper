#pragma once

#include <boost/json.hpp>
#include <boost/smart_ptr.hpp>
#include <map>
#include <string>

namespace copper::components::validator {

    class instance : public std::enable_shared_from_this<instance> {
    public:
        /**
         * Errors container
         */
        boost::json::object errors;

        /**
         * Status
         */
        bool success = false;

        /**
         * Insert message to errors
         *
         * @param key
         * @param message
         */
        void insert_or_push(const std::string &key, const std::string &message);

        /**
         * Constructor
         */
        instance() {}
    };

    /**
     * Validator factory
     *
     * @param rules
     * @param value
     * @return boost::shared_ptr<instance> Instance
     */
    boost::shared_ptr<instance> make(const std::map<std::string, std::string> &rules, const boost::json::value &value);

}  // namespace copper::components::validator

