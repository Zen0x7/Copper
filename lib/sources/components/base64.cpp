//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/base64.hpp>

namespace copper::components {
std::string base64_encode(const std::string &input, const bool padding) {
  std::string _output;
  int _value = 0;
  int _value_b = -6;

  for (const unsigned char _character : input) {
    _value = (_value << 8) + _character;
    _value_b += 8;
    while (_value_b >= 0) {
      _output.push_back(base64_charset_[_value >> _value_b & 0x3F]);
      _value_b -= 6;
    }
  }
  if (_value_b > -6)
    _output.push_back(
        base64_charset_[((_value << 8) >> (_value_b + 8)) & 0x3F]);

  if (padding)
    while (_output.size() % 4 != 0) _output.push_back('=');

  return _output;
  // LCOV_EXCL_START
}
// LCOV_EXCL_STOP

std::string base64_decode(const std::string &input) {
  std::string _output;
  int _value = 0;
  int _value_b = -8;
  for (const char _character : input) {
    if (_character == '=') break;
    _value = (_value << 6) + base64_map_charset_.at(_character);
    _value_b += 6;
    if (_value_b >= 0) {
      _output.push_back(static_cast<char>(_value >> _value_b & 0xFF));
      _value_b -= 8;
    }
  }
  return _output;
  // LCOV_EXCL_START
}
// LCOV_EXCL_STOP

}  // namespace copper::components
