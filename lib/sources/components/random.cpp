#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <copper/components/base64url.hpp>
#include <copper/components/random.hpp>

namespace copper::components {

std::string random_string(const int size) {
  std::string _output;
  _output.reserve(size);

  boost::random::random_device _device;
  const boost::random::uniform_int_distribution<> _distribution(
      0, base64url_charset_.size() - 1);

  for (int i = 0; i < size; i++)
    _output.push_back(base64url_charset_[_distribution(_device)]);

  return _output;
}

}  // namespace copper::components