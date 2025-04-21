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
