#pragma once

#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/smart_ptr.hpp>

namespace copper::components::expression {

    class result : public std::enable_shared_from_this<result> {
        bool matches_;
        std::unordered_map<std::string, std::string> bindings_;

    public:
        result(bool matches, const std::unordered_map<std::string, std::string> &bindings);
        bool matches() const;
        std::unordered_map<std::string, std::string> get_bindings() const;
        std::string get(const std::string &name) const;
    };

    class instance : public std::enable_shared_from_this<instance> {
        std::string regex_;
        std::vector<std::string> arguments_;

    public:
        instance(std::string regex, const std::vector<std::string> &arguments);
        std::vector<std::string> get_arguments() const;
        std::string get_regex() const;
        boost::shared_ptr<result> query(const std::string &input) const;
    };

    boost::shared_ptr<instance> from_string(const std::string &input);

}  // namespace copper::components::expressions
