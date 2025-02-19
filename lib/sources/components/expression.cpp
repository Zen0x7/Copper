#include <copper/components/expression.hpp>

namespace copper::components {
expression_result::expression_result(
    const bool matches,
    const std::unordered_map<std::string, std::string> &bindings)
    : matches_(matches), bindings_(bindings) {}

bool expression_result::matches() const { return matches_; }

std::unordered_map<std::string, std::string> expression_result::get_bindings()
    const {
  return bindings_;
}

std::string expression_result::get(const std::string &name) const {
  if (bindings_.contains(name)) return bindings_.at(name);

  // LCOV_EXCL_START
  std::string error_output = "Expression error: Parameter ";
  error_output.append(name);
  error_output.append(" doesn't has.");
  throw std::runtime_error(error_output.c_str());
}
// LCOV_EXCL_STOP

expression::expression(std::string regex,
                       const std::vector<std::string> &arguments)
    : regex_(std::move(regex)), arguments_(arguments) {}

std::vector<std::string> expression::get_arguments() const {
  return arguments_;
}

std::string expression::get_regex() const { return regex_; }

boost::shared_ptr<expression_result> expression::query(
    const std::string &input) const {
  std::unordered_map<std::string, std::string> _bindings;
  const std::regex _pattern(regex_);
  bool _matches = false;
  if (std::smatch _match; std::regex_match(input, _match, _pattern)) {
    _matches = true;
    auto _iterator = _match.begin();
    ++_iterator;
    for (auto &_key : arguments_) {
      _bindings[_key] = *_iterator;
      ++_iterator;
    }
  }
  return boost::make_shared<expression_result>(_matches, _bindings);
}

boost::shared_ptr<expression> expression_make(const std::string &input) {
  std::size_t _open = input.find('{');
  std::size_t _close = input.find('}');
  std::size_t _position = 0;

  std::vector<std::string> _arguments;
  std::string _regex;

  if (_open == std::string::npos && _close == std::string::npos)
    return boost::make_shared<expression>(input, _arguments);

  while (_open != std::string::npos && _close != std::string::npos) {
    _regex.append(input.substr(_position, _open - _position));
    std::string _value{input.substr(_open + 1, _close - _open - 1)};

    // LCOV_EXCL_START
    if (std::find(_arguments.begin(), _arguments.end(), _value) !=
        _arguments.end()) {
      std::string error_output = "Expression error: Argument ";
      error_output.append(_value);
      error_output.append(" already has.");
      throw std::runtime_error(error_output.c_str());
    }
    // LCOV_EXCL_STOP

    _regex.append(R"(([a-zA-Z0-9\-_]+))");
    _arguments.emplace_back(_value);

    _position = _close + 1;
    _open = input.find('{', _close);
    _close = input.find('}', _open);
  }

  if (_position != input.size())
    _regex.append(input.substr(_position, input.size() - _position));

  return boost::make_shared<expression>(_regex, _arguments);
}

}  // namespace copper::components
