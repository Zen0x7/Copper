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

#include <copper/components/expression.hpp>

namespace copper::components {
std::string expression_result::get(const std::string& name) const {
  if (bindings_.contains(name))
    return bindings_.at(name);

  std::string _error_output = "Expression error: Parameter ";
  _error_output.append(name);
  _error_output.append(" doesn't has.");
  throw expression_exception(_error_output);
}

shared<expression_result> expression::query(const std::string& input) const {
  unordered_map_of_strings _bindings;
  bool _matches = false;
  if (std::smatch _match; std::regex_match(input, _match, pattern_)) {
    _matches = true;
    auto _iterator = _match.begin();
    ++_iterator;
    for (auto& _key : arguments_) {
      _bindings[_key] = *_iterator;
      ++_iterator;
    }
  }
  return boost::make_shared<expression_result>(_matches, _bindings);
}

shared<expression> expression_make(const std::string& input) {
  std::size_t _open = input.find('{');
  std::size_t _close = input.find('}');
  std::size_t _position = 0;

  vector_of<std::string> _arguments;
  std::string _regex;

  if (_open == std::string::npos && _close == std::string::npos)
    return boost::make_shared<expression>(input, _arguments);

  while (_open != std::string::npos && _close != std::string::npos) {
    _regex.append(input.substr(_position, _open - _position));
    std::string _value{input.substr(_open + 1, _close - _open - 1)};

    if (std::ranges::find(_arguments, _value) != _arguments.end()) {
      std::string error_output = "Expression error: Argument ";
      error_output.append(_value);
      error_output.append(" already has.");
      throw expression_exception(error_output);
    }

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
