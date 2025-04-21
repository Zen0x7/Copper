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

#include <gtest/gtest.h>

#include <boost/algorithm/hex.hpp>
#include <copper/components/codec.hpp>

using namespace copper::components;

TEST(Components_Codec, Decode) {
  std::string hex_data =
      "000000000000004A8E010000016B412CEE00010000000000000000000000000000000001"
      "0005000100010100010011001D00010010015E2C880002000B000000003544C87A000E00"
      "0000001DD7E06A00000100002994";

  std::vector<unsigned char> binary_data;
  boost::algorithm::unhex(hex_data.begin(), hex_data.end(),
                          std::back_inserter(binary_data));

  ASSERT_FALSE(binary_data.empty());

  auto result = codec::codec_parse(binary_data.data());
  std::cout << "CODEC_ID: " << result->codec_id_ << std::endl;
  std::cout << "CRC: " << result->crc_ << std::endl;
  std::cout << "DATA_FIELD_LENGTH: " << result->data_field_length_ << std::endl;
  std::cout << "NUMBER_OF_DATA_1: " << result->number_of_data_1_ << std::endl;
  std::cout << "NUMBER_OF_DATA_2: " << result->number_of_data_2_ << std::endl;

  int i = 0;
  for (const auto& data : result->records_) {
    i++;
    std::cout << "DATA N° " << i << std::endl << "==============" << std::endl;
    std::cout << "ID: " << data->id_ << std::endl;
    std::cout << "LATITUDE: " << data->latitude_ << std::endl;
    std::cout << "LONGITUDE: " << data->longitude_ << std::endl;
    std::cout << "ALTITUDE: " << data->altitude_ << std::endl;
    std::cout << "ANGLE: " << data->angle_ << std::endl;
    std::cout << "TOTAL_IO: " << data->number_of_total_id_ << std::endl;
    std::cout << "PRIORITY: " << data->priority_ << std::endl;
    std::cout << "SATELLITES: " << data->satellites_ << std::endl;
    std::cout << "SPEED: " << data->speed_ << std::endl;
    std::cout << "TIMESTAMP: " << data->timestamp_ << std::endl;
    std::cout << "IO: " << serialize(data->extras_) << std::endl;
  }

  ASSERT_TRUE(result);  // Dependiendo de lo que codec_parse devuelva
}
