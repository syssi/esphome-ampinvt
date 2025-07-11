#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/ampinvt_modbus/ampinvt_modbus.h"

namespace esphome {
namespace ampinvt {

class Ampinvt : public PollingComponent, public ampinvt_modbus::AmpinvtModbusDevice {
 public:
  void set_operating_status_binary_sensor(binary_sensor::BinarySensor *operating_status_binary_sensor) {
    operating_status_binary_sensor_ = operating_status_binary_sensor;
  }
  void set_battery_status_binary_sensor(binary_sensor::BinarySensor *battery_status_binary_sensor) {
    battery_status_binary_sensor_ = battery_status_binary_sensor;
  }
  void set_fan_status_binary_sensor(binary_sensor::BinarySensor *fan_status_binary_sensor) {
    fan_status_binary_sensor_ = fan_status_binary_sensor;
  }
  void set_overheat_status_binary_sensor(binary_sensor::BinarySensor *overheat_status_binary_sensor) {
    overheat_status_binary_sensor_ = overheat_status_binary_sensor;
  }
  void set_dc_output_status_binary_sensor(binary_sensor::BinarySensor *dc_output_status_binary_sensor) {
    dc_output_status_binary_sensor_ = dc_output_status_binary_sensor;
  }
  void set_internal_temperature1_status_binary_sensor(
      binary_sensor::BinarySensor *internal_temperature1_status_binary_sensor) {
    internal_temperature1_status_binary_sensor_ = internal_temperature1_status_binary_sensor;
  }
  void set_internal_temperature2_status_binary_sensor(
      binary_sensor::BinarySensor *internal_temperature2_status_binary_sensor) {
    internal_temperature2_status_binary_sensor_ = internal_temperature2_status_binary_sensor;
  }
  void set_external_temperature_status_binary_sensor(
      binary_sensor::BinarySensor *external_temperature_status_binary_sensor) {
    external_temperature_status_binary_sensor_ = external_temperature_status_binary_sensor;
  }
  void set_charging_status_binary_sensor(binary_sensor::BinarySensor *charging_status_binary_sensor) {
    charging_status_binary_sensor_ = charging_status_binary_sensor;
  }
  void set_equal_charging_status_binary_sensor(binary_sensor::BinarySensor *equal_charging_status_binary_sensor) {
    equal_charging_status_binary_sensor_ = equal_charging_status_binary_sensor;
  }
  void set_mppt_tracking_status_binary_sensor(binary_sensor::BinarySensor *mppt_tracking_status_binary_sensor) {
    mppt_tracking_status_binary_sensor_ = mppt_tracking_status_binary_sensor;
  }
  void set_float_charging_status_binary_sensor(binary_sensor::BinarySensor *float_charging_status_binary_sensor) {
    float_charging_status_binary_sensor_ = float_charging_status_binary_sensor;
  }
  void set_charge_current_limit_status_binary_sensor(
      binary_sensor::BinarySensor *charge_current_limit_status_binary_sensor) {
    charge_current_limit_status_binary_sensor_ = charge_current_limit_status_binary_sensor;
  }
  void set_charge_derating_status_binary_sensor(binary_sensor::BinarySensor *charge_derating_status_binary_sensor) {
    charge_derating_status_binary_sensor_ = charge_derating_status_binary_sensor;
  }
  void set_remote_prohibit_charging_status_binary_sensor(
      binary_sensor::BinarySensor *remote_prohibit_charging_status_binary_sensor) {
    remote_prohibit_charging_status_binary_sensor_ = remote_prohibit_charging_status_binary_sensor;
  }
  void set_pv_overvoltage_status_binary_sensor(binary_sensor::BinarySensor *pv_overvoltage_status_binary_sensor) {
    pv_overvoltage_status_binary_sensor_ = pv_overvoltage_status_binary_sensor;
  }
  void set_charge_output_relay_status_binary_sensor(
      binary_sensor::BinarySensor *charge_output_relay_status_binary_sensor) {
    charge_output_relay_status_binary_sensor_ = charge_output_relay_status_binary_sensor;
  }
  void set_load_output_status_binary_sensor(binary_sensor::BinarySensor *load_output_status_binary_sensor) {
    load_output_status_binary_sensor_ = load_output_status_binary_sensor;
  }
  void set_fan_relay_status_binary_sensor(binary_sensor::BinarySensor *fan_relay_status_binary_sensor) {
    fan_relay_status_binary_sensor_ = fan_relay_status_binary_sensor;
  }
  void set_overcharge_protection_status_binary_sensor(
      binary_sensor::BinarySensor *overcharge_protection_status_binary_sensor) {
    overcharge_protection_status_binary_sensor_ = overcharge_protection_status_binary_sensor;
  }
  void set_overvoltage_protection_status_binary_sensor(
      binary_sensor::BinarySensor *overvoltage_protection_status_binary_sensor) {
    overvoltage_protection_status_binary_sensor_ = overvoltage_protection_status_binary_sensor;
  }
  void set_online_status_binary_sensor(binary_sensor::BinarySensor *online_status_binary_sensor) {
    online_status_binary_sensor_ = online_status_binary_sensor;
  }

  void set_pv_voltage_sensor(sensor::Sensor *pv_voltage_sensor) { pv_voltage_sensor_ = pv_voltage_sensor; }
  void set_battery_voltage_sensor(sensor::Sensor *battery_voltage_sensor) {
    battery_voltage_sensor_ = battery_voltage_sensor;
  }
  void set_charge_current_sensor(sensor::Sensor *charge_current_sensor) {
    charge_current_sensor_ = charge_current_sensor;
  }
  void set_mppt_temperature_sensor(sensor::Sensor *mppt_temperature_sensor) {
    mppt_temperature_sensor_ = mppt_temperature_sensor;
  }
  void set_battery_temperature_sensor(sensor::Sensor *battery_temperature_sensor) {
    battery_temperature_sensor_ = battery_temperature_sensor;
  }
  void set_today_yield_sensor(sensor::Sensor *today_yield_sensor) { today_yield_sensor_ = today_yield_sensor; }
  void set_generation_total_sensor(sensor::Sensor *generation_total_sensor) {
    generation_total_sensor_ = generation_total_sensor;
  }

  void set_operation_status_text_sensor(text_sensor::TextSensor *operation_status_text_sensor) {
    operation_status_text_sensor_ = operation_status_text_sensor;
  }

  void set_max_no_response_count(uint8_t max_no_response_count) { max_no_response_count_ = max_no_response_count; }

  void dump_config() override;

  void on_ampinvt_modbus_data(const std::vector<uint8_t> &data) override;

  void update() override;

 protected:
  binary_sensor::BinarySensor *operating_status_binary_sensor_;
  binary_sensor::BinarySensor *battery_status_binary_sensor_;
  binary_sensor::BinarySensor *fan_status_binary_sensor_;
  binary_sensor::BinarySensor *overheat_status_binary_sensor_;
  binary_sensor::BinarySensor *dc_output_status_binary_sensor_;
  binary_sensor::BinarySensor *internal_temperature1_status_binary_sensor_;
  binary_sensor::BinarySensor *internal_temperature2_status_binary_sensor_;
  binary_sensor::BinarySensor *external_temperature_status_binary_sensor_;
  binary_sensor::BinarySensor *charging_status_binary_sensor_;
  binary_sensor::BinarySensor *equal_charging_status_binary_sensor_;
  binary_sensor::BinarySensor *mppt_tracking_status_binary_sensor_;
  binary_sensor::BinarySensor *float_charging_status_binary_sensor_;
  binary_sensor::BinarySensor *charge_current_limit_status_binary_sensor_;
  binary_sensor::BinarySensor *charge_derating_status_binary_sensor_;
  binary_sensor::BinarySensor *remote_prohibit_charging_status_binary_sensor_;
  binary_sensor::BinarySensor *pv_overvoltage_status_binary_sensor_;
  binary_sensor::BinarySensor *charge_output_relay_status_binary_sensor_;
  binary_sensor::BinarySensor *load_output_status_binary_sensor_;
  binary_sensor::BinarySensor *fan_relay_status_binary_sensor_;
  binary_sensor::BinarySensor *overcharge_protection_status_binary_sensor_;
  binary_sensor::BinarySensor *overvoltage_protection_status_binary_sensor_;
  binary_sensor::BinarySensor *online_status_binary_sensor_;

  sensor::Sensor *pv_voltage_sensor_;
  sensor::Sensor *battery_voltage_sensor_;
  sensor::Sensor *charge_current_sensor_;
  sensor::Sensor *mppt_temperature_sensor_;
  sensor::Sensor *battery_temperature_sensor_;
  sensor::Sensor *today_yield_sensor_;
  sensor::Sensor *generation_total_sensor_;

  text_sensor::TextSensor *operation_status_text_sensor_;

  uint8_t no_response_count_ = 0;
  uint8_t max_no_response_count_ = 15;

  void on_status_data_(const std::vector<uint8_t> &data);
  void on_settings_data_(const std::vector<uint8_t> &data);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_device_unavailable_();
  void reset_online_status_tracker_();
  void track_online_status_();
};

}  // namespace ampinvt
}  // namespace esphome
