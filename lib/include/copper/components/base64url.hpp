#ifndef COPPER_COMPONENTS_BASE64URL_HPP
#define COPPER_COMPONENTS_BASE64URL_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/containers.hpp>
#include <map>
#include <string>

namespace copper::components {

/**
 * Encodes to Base64url
 *
 * @param input
 * @param padding
 * @return string
 */
std::string base64url_encode(const std::string &input, bool padding = true);

/**
 * Converts from Base64url
 *
 * @param input
 * @return string
 */
std::string base64url_decode(const std::string &input);

/**
 * Base64url charset
 */
const std::string base64url_charset_ =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

/**
 * Base64url charset as map
 */
inline const containers::map_of<char, int> base64url_map_charset_ = {
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

}  // namespace copper::components

#endif
