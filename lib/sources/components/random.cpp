#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <copper/components/base64url.hpp>
#include <copper/components/random.hpp>

namespace copper::components {

std::string random_string(int size) {
  std::string output;
  output.reserve(size);

  boost::random::random_device device;
  boost::random::uniform_int_distribution<> distribution(
      0, base64url_chars.size() - 1);

  for (int i = 0; i < size; i++)
    output.push_back(base64url_chars[distribution(device)]);

  return output;
}

}  // namespace copper::components