#include <copper/components/base64.hpp>

namespace copper::components {
std::string base64_encode(const std::string &input, bool padding) {
  std::string output;
  int value = 0;
  int value_b = -6;

  for (const unsigned char character : input) {
    value = (value << 8) + character;
    value_b += 8;
    while (value_b >= 0) {
      output.push_back(base64_charset_[(value >> value_b) & 0x3F]);
      value_b -= 6;
    }
  }
  if (value_b > -6)
    output.push_back(base64_charset_[((value << 8) >> (value_b + 8)) & 0x3F]);

  if (padding)
    while (output.size() % 4 != 0) output.push_back('=');

  return output;
  // LCOV_EXCL_START
}
// LCOV_EXCL_STOP

std::string base64_decode(const std::string &input) {
  std::string output;
  int value = 0;
  int value_b = -8;
  for (const char character : input) {
    if (character == '=') break;
    value = (value << 6) + base64_map_charset_[character];
    value_b += 6;
    if (value_b >= 0) {
      output.push_back(static_cast<char>((value >> value_b) & 0xFF));
      value_b -= 8;
    }
  }
  return output;
  // LCOV_EXCL_START
}
// LCOV_EXCL_STOP

}  // namespace copper::components
