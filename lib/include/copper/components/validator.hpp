#pragma once

#include <copper/components/json.hpp>

#include <copper/components/shared.hpp>
#include <copper/components/containers.hpp>
#include <map>
#include <string>

namespace copper::components {

    class validator : public std::enable_shared_from_this<validator> {
    public:
        /**
         * Errors container
         */
        json::object errors;

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
    shared<
            validator
    > validator_make(
            const containers::map_of_strings &rules,
            const json::value &value
    );

} // namespace copper::components::validator

