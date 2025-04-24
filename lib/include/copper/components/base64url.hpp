#ifndef COPPER_COMPONENTS_BASE64URL_HPP
#define COPPER_COMPONENTS_BASE64URL_HPP

#pragma once

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

#include <copper/components/containers.hpp>
#include <map>
#include <string>

namespace copper::components {
/**
 * Base64url charset
 */
const std::string base64url_charset_ =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01"
    "23456789-_";

/**
 * Base64url charset as map
 */
inline const map_of<char, int> base64url_map_charset_ = {
    {'A', 0},  {'B', 1},  {'C', 2},  {'D', 3},  {'E', 4},  {'F', 5},  {'G', 6},
    {'H', 7},  {'I', 8},  {'J', 9},  {'K', 10}, {'L', 11}, {'M', 12}, {'N', 13},
    {'O', 14}, {'P', 15}, {'Q', 16}, {'R', 17}, {'S', 18}, {'T', 19}, {'U', 20},
    {'V', 21}, {'W', 22}, {'X', 23}, {'Y', 24}, {'Z', 25}, {'a', 26}, {'b', 27},
    {'c', 28}, {'d', 29}, {'e', 30}, {'f', 31}, {'g', 32}, {'h', 33}, {'i', 34},
    {'j', 35}, {'k', 36}, {'l', 37}, {'m', 38}, {'n', 39}, {'o', 40}, {'p', 41},
    {'q', 42}, {'r', 43}, {'s', 44}, {'t', 45}, {'u', 46}, {'v', 47}, {'w', 48},
    {'x', 49}, {'y', 50}, {'z', 51}, {'0', 52}, {'1', 53}, {'2', 54}, {'3', 55},
    {'4', 56}, {'5', 57}, {'6', 58}, {'7', 59}, {'8', 60}, {'9', 61}, {'-', 62},
    {'_', 63}};

/**
 * Encodes to Base64url
 *
 * @param input
 * @param padding
 * @return string
 */
inline std::string base64url_encode(const std::string& input,
                                    bool padding = true) {
  std::string _output;
  int _value = 0;
  int _value_b = -6;

  for (const unsigned char _character : input) {
    _value = (_value << 8) + _character;
    _value_b += 8;
    while (_value_b >= 0) {
      _output.push_back(base64url_charset_[_value >> _value_b & 0x3F]);
      _value_b -= 6;
    }
  }
  if (_value_b > -6)
    _output.push_back(
        base64url_charset_[((_value << 8) >> (_value_b + 8)) & 0x3F]);

  if (padding)
    while (_output.size() % 4 != 0)
      _output.push_back('=');

  return _output;
}

/**
 * Converts from Base64url
 *
 * @param input
 * @return string
 */
inline std::string base64url_decode(const std::string& input) {
  std::string _output;
  int _value = 0;
  int _value_b = -8;
  for (const char _character : input) {
    if (_character == '=')
      break;
    _value = (_value << 6) + base64url_map_charset_.at(_character);
    _value_b += 6;
    if (_value_b >= 0) {
      _output.push_back(static_cast<char>(_value >> _value_b & 0xFF));
      _value_b -= 8;
    }
  }
  return _output;
}

}  // namespace copper::components

#endif
