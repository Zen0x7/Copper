#pragma once

#include <boost/json.hpp>
#include <boost/smart_ptr.hpp>
#include <map>
#include <string>

namespace copper::components {

    class validator : public std::enable_shared_from_this<validator> {
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
        void insert_or_push(
                const std::string &key,
                const std::string &message
        );

        /**
         * Constructor
         */
        validator() {}
    };

    /**
     * Validator factory
     *
     * @param rules
     * @param value
     * @return boost::shared_ptr<validator> validator
     */
    boost::shared_ptr<
            validator
    > validator_make(
            const std::map<
                    std::string,
                    std::string
            > &rules,
            const boost::json::value &value
    );

} // namespace copper::components::validator

