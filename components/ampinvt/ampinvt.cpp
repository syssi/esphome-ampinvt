#include "ampinvt.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ampinvt {

static const char *const TAG = "ampinvt";

static const uint8_t AMPINVT_COMMAND_STATUS = 0xB3;
static const uint8_t AMPINVT_COMMAND_SETTINGS = 0xB2;
static const uint8_t ANENJI_COMMAND_STATUS = 0xA3;
static const uint8_t ANENJI_COMMAND_SETTINGS = 0xA2;
static const uint8_t ANENJI_COMMAND_WRITE_PARAMETER = 0xDB;
static const uint8_t ANENJI_COMMAND_ERROR = 0xEE;

static const char *const OPERATION_STATUS_CHARGING = "Charging";
static const char *const OPERATION_STATUS_NOT_CHARGING = "Not Charging";
static const char *const OPERATION_STATUS_OFFLINE = "Offline";

static constexpr const char *const BATTERY_TYPES[] = {"Lead-acid maintenance free", "Lead-acid Gel", "Lead-acid liquid",
                                                      "Lithium"};
static constexpr const char *const LOAD_CONTROL[] = {"Off", "Auto", "Time control", "Light control", "Remote control"};
static constexpr const char *const BAUD_RATES[] = {"", "1200", "2400", "4800", "9600"};

namespace OperationStatusBits {
static const uint8_t OPERATING = 0x1;               // 00000001
static const uint8_t BATTERY = 0x2;                 // 00000010
static const uint8_t FAN = 0x4;                     // 00000100
static const uint8_t TEMPERATURE = 0x8;             // 00001000
static const uint8_t DC_OUTPUT = 0x10;              // 00010000
static const uint8_t INTERNAL_TEMPERATURE1 = 0x20;  // 00100000
static const uint8_t INTERNAL_TEMPERATURE2 = 0x40;  // 01000000
static const uint8_t EXTERNAL_TEMPERATURE = 0x80;   // 10000000
}  // namespace OperationStatusBits

namespace ChargingStatusBits {
static const uint8_t CHARGING = 0x1;                   // 00000001
static const uint8_t EQUAL_CHARGING = 0x2;             // 00000010
static const uint8_t MPPT_TRACKING = 0x4;              // 00000100
static const uint8_t FLOAT_CHARGING = 0x8;             // 00001000
static const uint8_t CHARGE_CURRENT_LIMIT = 0x10;      // 00010000
static const uint8_t CHARGE_DERATING = 0x20;           // 00100000
static const uint8_t REMOTE_PROHIBIT_CHARGING = 0x40;  // 01000000
static const uint8_t PV_OVERVOLTAGE = 0x80;            // 10000000
}  // namespace ChargingStatusBits

namespace ControlStatusBits {
static const uint8_t CHARGE_OUTPUT_RELAY = 0x1;      // 00000001
static const uint8_t LOAD_OUTPUT = 0x2;              // 00000010
static const uint8_t FAN_RELAY = 0x4;                // 00000100
static const uint8_t OVERCHARGE_PROTECTION = 0x10;   // 00010000
static const uint8_t OVERVOLTAGE_PROTECTION = 0x20;  // 00100000
}  // namespace ControlStatusBits

void Ampinvt::on_ampinvt_modbus_data(const std::vector<uint8_t> &data) {
  uint8_t function = data[1];

  this->reset_online_status_tracker_();

  switch (function) {
    case AMPINVT_COMMAND_STATUS:
      this->on_ampinvt_status_data_(data);
      this->send(AMPINVT_COMMAND_SETTINGS, 0x00, 0x00);
      return;
    case AMPINVT_COMMAND_SETTINGS:
      this->on_ampinvt_settings_data_(data);
      return;
    case ANENJI_COMMAND_STATUS:
      this->on_anenji_status_data_(data);
      this->send(ANENJI_COMMAND_SETTINGS, 0x00, 0x00);
      return;
    case ANENJI_COMMAND_SETTINGS:
      this->on_anenji_settings_data_(data);
      return;
    case ANENJI_COMMAND_WRITE_PARAMETER:
      ESP_LOGI(TAG, "Write parameter acknowledged");
      return;
    case ANENJI_COMMAND_ERROR:
      ESP_LOGW(TAG, "Error response: code=0x%02X command=0x%02X control=0x%02X", data[2], data[3], data[4]);
      return;
    default:
      ESP_LOGW(TAG, "Unsupported function code 0x%02X in frame: %s", function,
               format_hex_pretty(&data.front(), data.size()).c_str());  // NOLINT
      break;
  }
}

void Ampinvt::on_ampinvt_status_data_(const std::vector<uint8_t> &data) {
  ESP_LOGI(TAG, "Ampinvt status frame (%d bytes) received", data.size());
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());  // NOLINT

  if (data.size() < 37) {
    ESP_LOGW(TAG, "Invalid data size for status frame: %zu", data.size());
    return;
  }

  auto ampinvt_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };
  auto ampinvt_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(ampinvt_get_16bit(i + 0)) << 16) | (uint32_t(ampinvt_get_16bit(i + 2)) << 0);
  };

  // Parse bit status fields from bytes 3, 4, 5 (data[3], data[4], data[5])
  uint8_t op_status_byte = data[3];
  this->publish_state_(this->operating_status_binary_sensor_, (bool) (op_status_byte & OperationStatusBits::OPERATING));
  this->publish_state_(this->battery_status_binary_sensor_, (bool) (op_status_byte & OperationStatusBits::BATTERY));
  this->publish_state_(this->fan_status_binary_sensor_, (bool) (op_status_byte & OperationStatusBits::FAN));
  this->publish_state_(this->overheat_status_binary_sensor_,
                       (bool) (op_status_byte & OperationStatusBits::TEMPERATURE));
  this->publish_state_(this->dc_output_status_binary_sensor_, (bool) (op_status_byte & OperationStatusBits::DC_OUTPUT));
  this->publish_state_(this->internal_temperature1_status_binary_sensor_,
                       (bool) (op_status_byte & OperationStatusBits::INTERNAL_TEMPERATURE1));
  this->publish_state_(this->internal_temperature2_status_binary_sensor_,
                       (bool) (op_status_byte & OperationStatusBits::INTERNAL_TEMPERATURE2));
  this->publish_state_(this->external_temperature_status_binary_sensor_,
                       (bool) (op_status_byte & OperationStatusBits::EXTERNAL_TEMPERATURE));

  uint8_t chg_status_byte = data[4];
  this->publish_state_(this->charging_status_binary_sensor_, (bool) (chg_status_byte & ChargingStatusBits::CHARGING));
  this->publish_state_(this->equal_charging_status_binary_sensor_,
                       (bool) (chg_status_byte & ChargingStatusBits::EQUAL_CHARGING));
  this->publish_state_(this->mppt_tracking_status_binary_sensor_,
                       (bool) (chg_status_byte & ChargingStatusBits::MPPT_TRACKING));
  this->publish_state_(this->float_charging_status_binary_sensor_,
                       (bool) (chg_status_byte & ChargingStatusBits::FLOAT_CHARGING));
  this->publish_state_(this->charge_current_limit_status_binary_sensor_,
                       (bool) (chg_status_byte & ChargingStatusBits::CHARGE_CURRENT_LIMIT));
  this->publish_state_(this->charge_derating_status_binary_sensor_,
                       (bool) (chg_status_byte & ChargingStatusBits::CHARGE_DERATING));
  this->publish_state_(this->remote_prohibit_charging_status_binary_sensor_,
                       (bool) (chg_status_byte & ChargingStatusBits::REMOTE_PROHIBIT_CHARGING));
  this->publish_state_(this->pv_overvoltage_status_binary_sensor_,
                       (bool) (chg_status_byte & ChargingStatusBits::PV_OVERVOLTAGE));

  uint8_t ctl_status_byte = data[5];
  this->publish_state_(this->charge_output_relay_status_binary_sensor_,
                       (bool) (ctl_status_byte & ControlStatusBits::CHARGE_OUTPUT_RELAY));
  this->publish_state_(this->load_output_status_binary_sensor_,
                       (bool) (ctl_status_byte & ControlStatusBits::LOAD_OUTPUT));
  this->publish_state_(this->fan_relay_status_binary_sensor_, (bool) (ctl_status_byte & ControlStatusBits::FAN_RELAY));
  this->publish_state_(this->overcharge_protection_status_binary_sensor_,
                       (bool) (ctl_status_byte & ControlStatusBits::OVERCHARGE_PROTECTION));
  this->publish_state_(this->overvoltage_protection_status_binary_sensor_,
                       (bool) (ctl_status_byte & ControlStatusBits::OVERVOLTAGE_PROTECTION));

  this->publish_state_(this->pv_voltage_sensor_, ampinvt_get_16bit(6) * 0.1f);

  float battery_voltage = ampinvt_get_16bit(8) * 0.01f;
  float charge_current = ampinvt_get_16bit(10) * 0.01f;
  this->publish_state_(this->battery_voltage_sensor_, battery_voltage);
  this->publish_state_(this->charge_current_sensor_, charge_current);
  this->publish_state_(this->charging_power_sensor_, battery_voltage * charge_current);

  this->publish_state_(this->mppt_temperature_sensor_, ((int16_t) ampinvt_get_16bit(12)) * 0.1f);

  this->publish_state_(this->battery_temperature_sensor_, ((int16_t) ampinvt_get_16bit(16)) * 0.1f);

  this->publish_state_(this->today_yield_sensor_, (float) ampinvt_get_32bit(20));
  this->publish_state_(this->generation_total_sensor_, (float) ampinvt_get_32bit(24) * 0.001f);

  if (chg_status_byte & ChargingStatusBits::CHARGING) {
    this->publish_state_(this->operation_status_text_sensor_, OPERATION_STATUS_CHARGING);
  } else {
    this->publish_state_(this->operation_status_text_sensor_, OPERATION_STATUS_NOT_CHARGING);
  }
}

void Ampinvt::on_ampinvt_settings_data_(const std::vector<uint8_t> &data) {
  ESP_LOGI(TAG, "Ampinvt settings frame (%d bytes) received", data.size());
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());  // NOLINT

  if (data.size() < 64) {
    ESP_LOGW(TAG, "Invalid data size: %zu", data.size());
    return;
  }

  auto ampinvt_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  uint8_t battery_type = data[3];
  uint8_t load_ctrl = data[6];
  uint8_t baud = data[8];

  ESP_LOGI(TAG, "Settings:");
  ESP_LOGI(TAG, "  Battery type:        %s (%u)", battery_type < 4 ? BATTERY_TYPES[battery_type] : "Unknown",
           battery_type);
  ESP_LOGI(TAG, "  Identification:      %s", data[4] == 0 ? "Auto recognition" : "Manual setting");
  ESP_LOGI(TAG, "  Number of batteries: %u", data[5]);
  ESP_LOGI(TAG, "  Load control:        %s (%u)", load_ctrl < 5 ? LOAD_CONTROL[load_ctrl] : "Unknown", load_ctrl);
  ESP_LOGI(TAG, "  Local address:       %u", data[7]);
  ESP_LOGI(TAG, "  Baud rate:           %s bps", baud >= 1 && baud <= 4 ? BAUD_RATES[baud] : "Unknown");
  ESP_LOGI(TAG, "  Rated voltage:       %.2f V", ampinvt_get_16bit(9) * 0.01f);
  ESP_LOGI(TAG, "  Equal charge V max:  %.2f V", ampinvt_get_16bit(11) * 0.01f);
  ESP_LOGI(TAG, "  Float charge V max:  %.2f V", ampinvt_get_16bit(13) * 0.01f);
  ESP_LOGI(TAG, "  Discharge V min:     %.2f V", ampinvt_get_16bit(15) * 0.01f);
  ESP_LOGI(TAG, "  HW max charge curr:  %.2f A", ampinvt_get_16bit(17) * 0.01f);
  ESP_LOGI(TAG, "  Max charge curr:     %.2f A", ampinvt_get_16bit(19) * 0.01f);
  ESP_LOGI(TAG, "  Running charge curr: %.2f A", ampinvt_get_16bit(21) * 0.01f);
  ESP_LOGI(TAG, "  Model code:          0x%02X", data[23]);
  ESP_LOGI(TAG, "  Over-discharge rec:  %.2f V", ampinvt_get_16bit(25) * 0.01f);
  ESP_LOGI(TAG, "  OV protection V:     %.2f V", ampinvt_get_16bit(27) * 0.01f);
  ESP_LOGI(TAG, "  OV recovery V:       %.2f V", ampinvt_get_16bit(29) * 0.01f);
  ESP_LOGI(TAG, "  Light ctrl ON  PV:   %u V", ampinvt_get_16bit(31));
  ESP_LOGI(TAG, "  Light ctrl OFF PV:   %u V", ampinvt_get_16bit(33));
  ESP_LOGI(TAG, "  Delay turn-on:       %u s", ampinvt_get_16bit(35));
  ESP_LOGI(TAG, "  Delay turn-off:      %u s", ampinvt_get_16bit(37));
}

void Ampinvt::on_anenji_status_data_(const std::vector<uint8_t> &data) {
  ESP_LOGI(TAG, "Anenji status frame (%d bytes) received", data.size());
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());  // NOLINT

  if (data.size() < 21) {
    ESP_LOGW(TAG, "Invalid data size for status frame: %zu", data.size());
    return;
  }

  auto ampinvt_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  uint8_t op_status_byte = data[3];
  this->publish_state_(this->operating_status_binary_sensor_, (bool) (op_status_byte & OperationStatusBits::OPERATING));
  this->publish_state_(this->battery_status_binary_sensor_, (bool) (op_status_byte & OperationStatusBits::BATTERY));
  this->publish_state_(this->fan_status_binary_sensor_, (bool) (op_status_byte & OperationStatusBits::FAN));
  this->publish_state_(this->overheat_status_binary_sensor_,
                       (bool) (op_status_byte & OperationStatusBits::TEMPERATURE));
  this->publish_state_(this->dc_output_status_binary_sensor_, (bool) (op_status_byte & OperationStatusBits::DC_OUTPUT));

  uint8_t chg_status_byte = data[4];
  this->publish_state_(this->charging_status_binary_sensor_, (bool) (chg_status_byte & ChargingStatusBits::CHARGING));
  this->publish_state_(this->mppt_tracking_status_binary_sensor_,
                       (bool) (chg_status_byte & ChargingStatusBits::MPPT_TRACKING));
  this->publish_state_(this->float_charging_status_binary_sensor_,
                       (bool) (chg_status_byte & ChargingStatusBits::FLOAT_CHARGING));
  this->publish_state_(this->remote_prohibit_charging_status_binary_sensor_,
                       (bool) (chg_status_byte & ChargingStatusBits::REMOTE_PROHIBIT_CHARGING));
  this->publish_state_(this->pv_overvoltage_status_binary_sensor_,
                       (bool) (chg_status_byte & ChargingStatusBits::PV_OVERVOLTAGE));

  uint8_t ctl_status_byte = data[5];
  this->publish_state_(this->charge_output_relay_status_binary_sensor_,
                       (bool) (ctl_status_byte & ControlStatusBits::CHARGE_OUTPUT_RELAY));
  this->publish_state_(this->load_output_status_binary_sensor_,
                       (bool) (ctl_status_byte & ControlStatusBits::LOAD_OUTPUT));
  this->publish_state_(this->fan_relay_status_binary_sensor_, (bool) (ctl_status_byte & ControlStatusBits::FAN_RELAY));

  this->publish_state_(this->pv_voltage_sensor_, ampinvt_get_16bit(6) * 0.1f);

  float battery_voltage = ampinvt_get_16bit(8) * 0.01f;
  float charge_current = ampinvt_get_16bit(10) * 0.01f;
  this->publish_state_(this->battery_voltage_sensor_, battery_voltage);
  this->publish_state_(this->charge_current_sensor_, charge_current);
  this->publish_state_(this->charging_power_sensor_, battery_voltage * charge_current);

  this->publish_state_(this->mppt_temperature_sensor_, ((int16_t) ampinvt_get_16bit(12)) * 0.1f);

  this->publish_state_(this->battery_temperature_sensor_, ((int16_t) ampinvt_get_16bit(16)) * 0.1f);

  if (chg_status_byte & ChargingStatusBits::CHARGING) {
    this->publish_state_(this->operation_status_text_sensor_, OPERATION_STATUS_CHARGING);
  } else {
    this->publish_state_(this->operation_status_text_sensor_, OPERATION_STATUS_NOT_CHARGING);
  }
}

void Ampinvt::on_anenji_settings_data_(const std::vector<uint8_t> &data) {
  ESP_LOGI(TAG, "Anenji settings frame (%d bytes) received", data.size());
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());  // NOLINT

  if (data.size() < 26) {
    ESP_LOGW(TAG, "Invalid data size: %zu", data.size());
    return;
  }

  auto ampinvt_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  uint8_t battery_type = data[3];
  uint8_t load_ctrl = data[6];
  uint8_t baud = data[8];

  ESP_LOGI(TAG, "Settings:");
  ESP_LOGI(TAG, "  Battery type:        %s (%u)", battery_type < 4 ? BATTERY_TYPES[battery_type] : "Unknown",
           battery_type);
  ESP_LOGI(TAG, "  Identification:      %s", data[4] == 0 ? "Auto recognition" : "Manual setting");
  ESP_LOGI(TAG, "  Number of batteries: %u", data[5]);
  ESP_LOGI(TAG, "  Load control:        %s (%u)", load_ctrl < 5 ? LOAD_CONTROL[load_ctrl] : "Unknown", load_ctrl);
  ESP_LOGI(TAG, "  Local address:       %u", data[7]);
  ESP_LOGI(TAG, "  Baud rate:           %s bps", baud >= 1 && baud <= 4 ? BAUD_RATES[baud] : "Unknown");

  ESP_LOGI(TAG, "  Rated voltage:       %.2f V", ampinvt_get_16bit(9) * 0.01f);
  this->publish_state_(this->rated_voltage_sensor_, ampinvt_get_16bit(9) * 0.01f);

  ESP_LOGI(TAG, "  Equal charge V max:  %.2f V", ampinvt_get_16bit(11) * 0.01f);
  ESP_LOGI(TAG, "  Float charge V max:  %.2f V", ampinvt_get_16bit(13) * 0.01f);
  ESP_LOGI(TAG, "  Discharge V min:     %.2f V", ampinvt_get_16bit(15) * 0.01f);
  ESP_LOGI(TAG, "  HW max charge curr:  %.2f A", ampinvt_get_16bit(17) * 0.01f);

  ESP_LOGI(TAG, "  Max charge curr:     %.2f A", ampinvt_get_16bit(19) * 0.01f);
  this->publish_state_(this->max_charge_current_limit_sensor_, ampinvt_get_16bit(19) * 0.01f);

  ESP_LOGI(TAG, "  Running charge curr: %.2f A", ampinvt_get_16bit(21) * 0.01f);
}

void Ampinvt::publish_device_unavailable_() {
  this->publish_state_(this->online_status_binary_sensor_, false);
  this->publish_state_(this->operation_status_text_sensor_, OPERATION_STATUS_OFFLINE);

  // Set all sensors to NAN when device is unavailable
  this->publish_state_(this->pv_voltage_sensor_, NAN);
  this->publish_state_(this->battery_voltage_sensor_, NAN);
  this->publish_state_(this->charge_current_sensor_, NAN);
  this->publish_state_(this->mppt_temperature_sensor_, NAN);
  this->publish_state_(this->battery_temperature_sensor_, NAN);
  this->publish_state_(this->today_yield_sensor_, NAN);
  this->publish_state_(this->generation_total_sensor_, NAN);
  this->publish_state_(this->rated_voltage_sensor_, NAN);
  this->publish_state_(this->max_charge_current_limit_sensor_, NAN);
  this->publish_state_(this->charging_power_sensor_, NAN);
}

void Ampinvt::reset_online_status_tracker_() {
  this->no_response_count_ = 0;
  this->publish_state_(this->online_status_binary_sensor_, true);
}

void Ampinvt::track_online_status_() {
  // Skip offline tracking if max_no_response_count is 0
  if (this->max_no_response_count_ == 0) {
    return;
  }

  if (this->no_response_count_ < this->max_no_response_count_) {
    this->no_response_count_++;
  }
  if (this->no_response_count_ == this->max_no_response_count_) {
    this->publish_device_unavailable_();
    this->no_response_count_++;
  }
}

void Ampinvt::update() {
  this->track_online_status_();

  uint8_t cmd = (this->protocol_ == Protocol::ANENJI) ? ANENJI_COMMAND_STATUS : AMPINVT_COMMAND_STATUS;
  this->send(cmd, 0x00, 0x00);
}

void Ampinvt::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void Ampinvt::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void Ampinvt::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

void Ampinvt::dump_config() {
  ESP_LOGCONFIG(TAG, "Ampinvt:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  ESP_LOGCONFIG(TAG, "  Protocol: %s",
                (this->protocol_ == Protocol::ANENJI) ? "Anenji (0xA2/0xA3)" : "Ampinvt (0xB2/0xB3)");
  ESP_LOGCONFIG(TAG, "  Max no response count: %d", this->max_no_response_count_);

  LOG_BINARY_SENSOR("", "Operating Status", this->operating_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Battery Status", this->battery_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Fan Status", this->fan_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Overheat Status", this->overheat_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "DC Output Status", this->dc_output_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Internal Temperature 1 Status", this->internal_temperature1_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Internal Temperature 2 Status", this->internal_temperature2_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "External Temperature Status", this->external_temperature_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Charging Status", this->charging_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Equal Charging Status", this->equal_charging_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "MPPT Tracking Status", this->mppt_tracking_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Float Charging Status", this->float_charging_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Charge Current Limit Status", this->charge_current_limit_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Charge Derating Status", this->charge_derating_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Remote Prohibit Charging Status", this->remote_prohibit_charging_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "PV Overvoltage Status", this->pv_overvoltage_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Charge Output Relay Status", this->charge_output_relay_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Load Output Status", this->load_output_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Fan Relay Status", this->fan_relay_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Overcharge Protection Status", this->overcharge_protection_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Overvoltage Protection Status", this->overvoltage_protection_status_binary_sensor_);
  LOG_BINARY_SENSOR("", "Online Status", this->online_status_binary_sensor_);

  LOG_SENSOR("", "PV Voltage", this->pv_voltage_sensor_);
  LOG_SENSOR("", "Battery Voltage", this->battery_voltage_sensor_);
  LOG_SENSOR("", "Charge Current", this->charge_current_sensor_);
  LOG_SENSOR("", "MPPT Temperature", this->mppt_temperature_sensor_);
  LOG_SENSOR("", "Battery Temperature", this->battery_temperature_sensor_);
  LOG_SENSOR("", "Today Yield", this->today_yield_sensor_);
  LOG_SENSOR("", "Generation Total", this->generation_total_sensor_);
  LOG_SENSOR("", "Rated Voltage", this->rated_voltage_sensor_);
  LOG_SENSOR("", "Max Charge Current Limit", this->max_charge_current_limit_sensor_);
  LOG_SENSOR("", "Charging Power", this->charging_power_sensor_);

  LOG_TEXT_SENSOR("", "Operation Status", this->operation_status_text_sensor_);
}

}  // namespace ampinvt
}  // namespace esphome
