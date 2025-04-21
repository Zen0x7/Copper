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

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <copper/components/gunzip.hpp>
#include <sstream>

namespace copper::components {

std::string gunzip_compress(const std::string& input) {
  std::stringstream _stream;
  boost::iostreams::filtering_ostream _ostream;
  _ostream.push(boost::iostreams::gzip_compressor());
  _ostream.push(_stream);

  std::istringstream _istream(input);

  boost::iostreams::copy(_istream, _ostream);

  return _stream.str();
}

std::string gunzip_decompress(const std::string& input) {
  std::stringstream _cstream(input);
  std::stringstream _dstream;

  boost::iostreams::filtering_istream _fstream;
  _fstream.push(boost::iostreams::gzip_decompressor());
  _fstream.push(_cstream);

  boost::iostreams::copy(_fstream, _dstream);

  return _dstream.str();
}
}  // namespace copper::components
