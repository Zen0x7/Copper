#ifndef COPPER_COMPONENTS_CODEC_HPP
#define COPPER_COMPONENTS_CODEC_HPP

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

#include <chrono>
#include <copper/components/binaries.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/stream.hpp>
#include <regex>

namespace copper::components::codec {

/**
 * Convert codec IO_ID to string
 *
 * @param key
 * @return string
 */
// LCOV_EXCL_START
inline std::string codec_io_id_to_string(unsigned long key) {
  switch (key) {
    case 239:
      // 0 – Ignition Off
      // 1 – Ignition On
      return "ignition";
    case 240:
      // 0 – Movement Off
      // 1 – Movement On
      return "movement";
    case 80:
      // 0 – Home On Stop
      // 1 – Home On Moving
      // 2 – Roaming On Stop
      // 3 – Roaming On Moving
      // 4 – Unknown On Stop
      // 5 – Unknown On Moving
      return "data_mode";
    case 21:
      // Value in range 1-5
      return "gsm_signal";
    case 200:
      // 0 - No Sleep
      // 1 – GPS Sleep
      // 2 – Deep Sleep
      // 3 – Online Sleep
      // 4 - Ultra Sleep
      return "sleep_mode";
    case 69:
      // 0 - GNSS OFF
      // 1 – GNSS ON with fix
      // 2 - GNSS ON without fix
      // 3 - GNSS sleep
      return "gnss_status";
    case 181:
      // Coefficient, calculation formula
      // (https://wiki.teltonika-gps.com/view/FMB120_Status_info#GNSS_Info)
      return "gnss_pdop";
    case 182:
      // Coefficient, calculation formula
      // (https://wiki.teltonika-gps.com/view/FMB120_Status_info#GNSS_Info)
      return "gnss_hdop";
    case 66:
      // Voltage (V)
      return "external_voltage";
    case 24:
      // GNSS Speed (km/h)
      return "speed";
    case 205:
      // GSM base station ID
      return "gsm_cell_id";
    case 206:
      // Location Area code (LAC), it depends on GSM
      // operator. It provides unique number which
      // assigned to a set of base GSM stations.
      return "gsm_area_code";
    case 67:
      // Voltage (V)
      return "battery_voltage";
    case 68:
      // Current (A)
      return "battery_current";
    case 241:
      // Currently used GSM Operator code
      return "active_gsm_operator";
    case 199:
      // Trip Odometer value (m)
      return "trip_odometer";
    case 16:
      // Total Odometer value (m)
      return "total_odometer";
    case 1:
      // Logic (0/1)
      return "digital_input_1";
    case 9:
      // Voltage (mV)
      return "analog_input_1";
    case 179:
      // Logic (0/1)
      return "digital_output_1";
    case 12:
      // Fuel Used
      return "fuel_used_gps";
    case 13:
      // Average Fuel Use (L/100Km)
      return "fuel_rate_gps";
    case 17:
      // X axis value (mG)
      return "axis_x";
    case 18:
      // Y axis value (mG)
      return "axis_y";
    case 19:
      // Z axis value (mG)
      return "axis_z";
    case 11:
      // Value of SIM ICCID, MSB
      return "iccid1";
    case 10:
      // 0 not present - 1 present
      return "sd_status";
    case 2:
      // Logic (0/1)
      return "digital_input_2";
    case 3:
      // Logic (0/1)
      return "digital_input_3";
    case 6:
      // Voltage (mV)
      return "analog_input_2";
    case 180:
      // Logic (0/1)
      return "digital_output_2";
    case 72:
      // Degrees (°C), -55 to +115
      // if 850 – Sensor not ready
      // if 2000 – Value read error
      // if 3000 – Not connected
      // if 4000 – ID failed
      // if 5000 – same as 850
      return "dallas_temperature_1";
    case 73:
      // Degrees (°C), -55 to +115
      // if 850 – Sensor not ready
      // if 2000 – Value read error
      // if 3000 – Not connected
      // if 4000 – ID failed
      // if 5000 – same as 850
      return "dallas_temperature_2";
    case 74:
      // Degrees (°C), -55 to +115
      // if 850 – Sensor not ready
      // if 2000 – Value read error
      // if 3000 – Not connected
      // if 4000 – ID failed
      // if 5000 – same as 850
      return "dallas_temperature_3";
    case 75:
      // Degrees (°C), -55 to +115
      // if 850 – Sensor not ready
      // if 2000 – Value read error
      // if 3000 – Not connected
      // if 4000 – ID failed
      // if 5000 – same as 850
      return "dallas_temperature_4";
    case 76:
      // Dallas sensor ID
      return "dallas_temperature_id_1";
    case 77:
      // Dallas sensor ID
      return "dallas_temperature_id_2";
    case 79:
      // Dallas sensor ID
      return "dallas_temperature_id_3";
    case 71:
      // Dallas sensor ID
      return "dallas_temperature_id_4";
    case 78:
      // iButton ID
      return "ibutton";
    case 207:
      // RFID ID
      return "dallas_temperature_id_1";
    case 201:
      // Fuel level measured by LLS sensor via RS232/RS485
      // (kvants or ltr)
      return "lls_1_fuel_level";
    case 202:
      // Fuel temperature measured by LLS sensor via
      // RS232/RS485 (C°)
      return "lls_1_temperature";
    case 203:
      // Fuel level measured by LLS sensor via RS232/RS485
      // (kvants or ltr)
      return "lls_2_fuel_level";
    case 204:
      // Fuel temperature measured by LLS sensor via
      // RS232/RS485 (C°)
      return "lls_2_temperature";
    case 210:
      // Fuel level measured by LLS sensor via RS232/RS485
      // (kvants or ltr)
      return "lls_3_fuel_level";
    case 211:
      // Fuel temperature measured by LLS sensor via
      // RS232/RS485 (C°)
      return "lls_3_temperature";
    case 212:
      // Fuel level measured by LLS sensor via RS232/RS485
      // (kvants or ltr)
      return "lls_4_fuel_level";
    case 213:
      // Fuel temperature measured by LLS sensor via
      // RS232/RS485 (C°)
      return "lls_4_temperature";
    case 214:
      // Fuel level measured by LLS sensor via RS232/RS485
      // (kvants or ltr)
      return "lls_5_fuel_level";
    case 215:
      // Fuel temperature measured by LLS sensor via
      // RS232/RS485 (C°)
      return "lls_5_temperature";
    case 15:
      // Average amount of events on some distance
      return "eco_score";
    case 113:
      // Battery capacity level (%)
      return "battery_level";
    case 238:
      // MAC address of NMEA receiver device connected via
      // Bluetooth
      return "user_id";
    case 237:
      // 0 - 3G
      // 1 - GSM
      // 2 - 4G
      // 3 - LTE CAT M1
      // 4 - LTE CAT NB1
      // 99 - Unknown
      return "network_type";
    case 8:
      // If ID is shown in this I/O that means that
      // attached iButton is in iButton List
      return "authorized_ibutton";
    case 4:
      // Counts pulses, count is reset when records are
      // saved
      return "pulse_counter_din1";
    case 5:
      // Counts pulses, count is reset when records are
      // saved
      return "pulse_counter_din2";
    case 263:
      // 0 - BT is disabled
      // 1 - BT Enabled, not device connected
      // 2 - Device connected, BTv3 Only
      // 3 - Device connected, BLE only
      // 4 - Device connected, BLE + BT
      return "bt_status";
    case 264:
      // Barcode ID
      return "barcode_id";
    case 269:
      // Fuel temperature (C°)
      return "escort_lls_temperature_1";
    case 270:
      // Fuel Level
      return "escort_lls_fuel_level_1";
    case 271:
      // Battery Voltage (V)
      return "escort_lls_battery_voltage_1";
    case 272:
      // Fuel temperature (C°)
      return "escort_lls_temperature_2";
    case 273:
      // Fuel Level
      return "escort_lls_fuel_level_2";
    case 274:
      // Battery Voltage (V)
      return "escort_lls_battery_voltage_2";
    case 275:
      // Fuel temperature (C°)
      return "escort_lls_temperature_3";
    case 276:
      // Fuel Level
      return "escort_lls_fuel_level_3";
    case 277:
      // Battery Voltage (V)
      return "escort_lls_battery_voltage_3";
    case 278:
      // Fuel temperature (C°)
      return "escort_lls_temperature_4";
    case 279:
      // Fuel Level
      return "escort_lls_fuel_level_4";
    case 280:
      // Battery Voltage (V)
      return "escort_lls_battery_voltage_4";
    case 303:
      // Logic (0/1)
      return "instant_movement";
    case 327:
      // UL202-02 Sensor Fuel level (mm)
      return "ul202_02_sensor_fuel_level";
    case 483:
      // UL202-02 sensor status codes
      return "ul202_02_sensor_status_codes";
    case 380:
      // Logic (0/1)
      return "digital_output_3";
    case 381:
      // Logic (0/1)
      return "ground_sense";
    case 387:
      // Logic (0/1)
      return "iso6709_coordinates";
    case 0:
      return "location";
    default:
      return "not_found";
  }
}
// LCOV_EXCL_STOP

/**
 * Extract IMEI from data
 *
 * @param length
 * @param data
 * @return string
 */
// LCOV_EXCL_START
inline std::string codec_get_imei(std::size_t length, unsigned char* data) {
  if (length != 17) {
    return "NONE";
  }

  if (data[0] != 0x00 || data[1] != 0x0F) {
    return "NONE";
  }

  const std::string prompt(data, data + length);
  if (const std::regex regex("^\\d{15}$");
      std::regex_match(prompt.substr(2, 15), regex)) {
    return prompt.substr(2, 15);
  }

  return "NONE";
}
// LCOV_EXCL_STOP

/**
 * Parse stream
 *
 * @param data
 * @return shared<stream>
 */
inline shared<stream> codec_parse(const unsigned char* data) {
  const auto _stream = boost::make_shared<stream>();
  // LCOV_EXCL_START
  if (data[0] == 0xFF) {
    // KeepAlive
    return _stream;
  }
  // LCOV_EXCL_STOP

  using namespace binaries;

  std::size_t _pos = 0;

  _stream->data_field_length_ = four_bytes(data, 4);
  _pos += 8;  // from 4 to 8

  _stream->codec_id_ = static_cast<int>(one_byte(data, 8));
  _pos += 1;  // from 8 to 9

  _stream->number_of_data_1_ = static_cast<int>(one_byte(data, 9));
  _pos += 1;  // from 9 to 10

  for (std::size_t i = 0; i < _stream->number_of_data_1_; i++) {
    const auto _record = boost::make_shared<record>();

    _record->timestamp_ = eight_bytes(data, _pos);
    _pos += 8;  // from 10 to 18

    _record->priority_ = static_cast<int>(one_byte(data, _pos));
    _pos += 1;  // from 18 to 19

    _record->longitude_ =
        static_cast<float>(static_cast<int32_t>(four_bytes(data, _pos))) /
        10000000;
    _pos += 4;

    _record->latitude_ =
        static_cast<float>(static_cast<int32_t>(four_bytes(data, _pos))) /
        10000000;
    _pos += 4;

    _record->altitude_ = static_cast<long>(two_bytes(data, _pos));
    _pos += 2;

    _record->angle_ = static_cast<long>(two_bytes(data, _pos));
    _pos += 2;

    _record->satellites_ = static_cast<int>(one_byte(data, _pos));
    _pos += 1;

    _record->speed_ = static_cast<long>(two_bytes(data, _pos));
    _pos += 2;

    _record->event_io_id_ = static_cast<long>(two_bytes(data, _pos));
    _pos += 2;

    _record->number_of_total_id_ = static_cast<long>(two_bytes(data, _pos));
    _pos += 2;

    const std::size_t _n_o_b_i = two_bytes(data, _pos);
    _pos += 2;

    for (std::size_t _o_b_i = 0; _o_b_i < _n_o_b_i; _o_b_i++) {
      const auto io_id = static_cast<int>(two_bytes(data, _pos));
      _pos += 2;

      const auto io_value = static_cast<int>(one_byte(data, _pos));
      _pos += 1;

      _record->extras_[codec_io_id_to_string(io_id)] = io_value;
    }

    const std::size_t _n_t_b_i = two_bytes(data, _pos);
    _pos += 2;

    for (std::size_t _t_b_i = 0; _t_b_i < _n_t_b_i; _t_b_i++) {
      const auto _io_id = static_cast<int>(two_bytes(data, _pos));
      _pos += 2;

      const auto _io_value = static_cast<int>(two_bytes(data, _pos));
      _pos += 2;

      _record->extras_[codec_io_id_to_string(_io_id)] = _io_value;
    }

    const std::size_t _n_f_b_i = two_bytes(data, _pos);
    _pos += 2;

    for (std::size_t _f_b_i = 0; _f_b_i < _n_f_b_i; _f_b_i++) {
      const auto _io_id = static_cast<int>(two_bytes(data, _pos));
      _pos += 2;

      const auto _io_value = static_cast<int>(four_bytes(data, _pos));
      _pos += 4;

      _record->extras_[codec_io_id_to_string(_io_id)] = _io_value;
    }

    const std::size_t _n_e_b_i = two_bytes(data, _pos);
    _pos += 2;

    // LCOV_EXCL_START
    for (std::size_t _e_b_i = 0; _e_b_i < _n_e_b_i; _e_b_i++) {
      const auto io_id = static_cast<int>(two_bytes(data, _pos));
      _pos += 2;

      const auto io_value = static_cast<long>(two_bytes(data, _pos));
      _pos += 8;

      _record->extras_[codec_io_id_to_string(io_id)] = io_value;
    }
    // LCOV_EXCL_STOP

    const std::size_t _n_n_b_i = two_bytes(data, _pos);
    _pos += 2;

    // LCOV_EXCL_START
    for (std::size_t _n_b_i = 0; _n_b_i < _n_n_b_i; _n_b_i++) {
      _pos += 2;

      const auto _len = static_cast<int>(two_bytes(data, _pos));
      _pos += 2;

      _pos += _len;
    }
    // LCOV_EXCL_STOP

    _stream->records_.push_back(_record);
  }

  _stream->number_of_data_2_ = static_cast<int>(one_byte(data, 9));
  _pos += 1;

  const auto _crc_result = calculate_crc(data, _pos - 8);

  _stream->crc_ = static_cast<int>(four_bytes(data, _pos));

  if (_stream->crc_ == _crc_result) {
  } else {
  }

  return _stream;
}

}  // namespace copper::components::codec

#endif
