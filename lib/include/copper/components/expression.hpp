#pragma once

#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/smart_ptr.hpp>

namespace copper::components::expression {

    class result : public std::enable_shared_from_this<result> {
        /**
         * Matches flag
         */
        bool matches_;

        /**
         * Bindings
         */
        std::unordered_map<
                std::string,
                std::string
        > bindings_;

    public:

        /**
         * Constructor
         *
         * @param matches
         * @param bindings
         */
        result(
                bool matches,
                const std::unordered_map<
                        std::string,
                        std::string
                > &bindings
        );

        /**
         * Retrieves if matches
         *
         * @return
         */
        bool matches() const;

        /**
         * Retrieves the bindings
         *
         * @return
         */
        std::unordered_map<
                std::string,
                std::string
        > get_bindings() const;

        /**
         * Retrieves the value of attribute
         *
         * @param name
         * @return
         */
        std::string get(
                const std::string &name
        ) const;
    };

    class instance : public std::enable_shared_from_this<instance> {

        /**
         * Regex expression
         */
        std::string regex_;

        /**
         * Arguments
         */
        std::vector<
                std::string
        > arguments_;

    public:

        /**
         * Constructor
         *
         * @param regex
         * @param arguments
         */
        instance(
                std::string regex,
                const std::vector<
                        std::string
                > &arguments
        );

        /**
         * Retrieves arguments
         *
         * @return
         */
        std::vector<
                std::string
        > get_arguments() const;

        /**
         * Retrieves regex expression
         * @return
         */
        std::string get_regex() const;

        /**
         * Retrieves a result
         *
         * @param input
         * @return
         */
        boost::shared_ptr<
                result
        > query(
                const std::string &input
        ) const;
    };

    /**
     * Factory
     *
     * @param input
     * @return
     */
    boost::shared_ptr<
            instance
    > from_string(
            const std::string &input
    );

}  // namespace copper::components::expressions
