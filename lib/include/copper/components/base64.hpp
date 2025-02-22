#ifndef COPPER_COMPONENTS_BASE64_HPP
#define COPPER_COMPONENTS_BASE64_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <map>
#include <string>

namespace copper::components {

/**
 * Encodes to Base64
 *
 * @param input
 * @param padding
 * @return string
 */
std::string base64_encode(const std::string &input, bool padding = true);

/**
 * Decodes from Base64
 *
 * @param input
 * @return string
 */
std::string base64_decode(const std::string &input);

/**
 * Base64 charset
 */
const std::string base64_charset_ =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * Base64 charset as map
 */
inline containers::map_of<char, int> base64_map_charset_ = {
    {'A', 0},  {'B', 1},  {'C', 2},  {'D', 3},  {'E', 4},  {'F', 5},  {'G', 6},
    {'H', 7},  {'I', 8},  {'J', 9},  {'K', 10}, {'L', 11}, {'M', 12}, {'N', 13},
    {'O', 14}, {'P', 15}, {'Q', 16}, {'R', 17}, {'S', 18}, {'T', 19}, {'U', 20},
    {'V', 21}, {'W', 22}, {'X', 23}, {'Y', 24}, {'Z', 25}, {'a', 26}, {'b', 27},
    {'c', 28}, {'d', 29}, {'e', 30}, {'f', 31}, {'g', 32}, {'h', 33}, {'i', 34},
    {'j', 35}, {'k', 36}, {'l', 37}, {'m', 38}, {'n', 39}, {'o', 40}, {'p', 41},
    {'q', 42}, {'r', 43}, {'s', 44}, {'t', 45}, {'u', 46}, {'v', 47}, {'w', 48},
    {'x', 49}, {'y', 50}, {'z', 51}, {'0', 52}, {'1', 53}, {'2', 54}, {'3', 55},
    {'4', 56}, {'5', 57}, {'6', 58}, {'7', 59}, {'8', 60}, {'9', 61}, {'+', 62},
    {'/', 63}};

}  // namespace copper::components

#endif