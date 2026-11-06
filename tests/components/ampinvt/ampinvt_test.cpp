#include <gtest/gtest.h>
#include "common.h"
#include "frames.h"

namespace esphome::ampinvt::testing {

// ── Ampinvt B3 status frame ───────────────────────────────────────────────────

TEST(AmpinvtStatusTest, AnalogSensors) {
  TestableAmpinvt bms;
  sensor::Sensor pv, bat, curr, mppt_temp, bat_temp, today, gen_total, power;
  bms.set_pv_voltage_sensor(&pv);
  bms.set_battery_voltage_sensor(&bat);
  bms.set_charge_current_sensor(&curr);
  bms.set_mppt_temperature_sensor(&mppt_temp);
  bms.set_battery_temperature_sensor(&bat_temp);
  bms.set_today_yield_sensor(&today);
  bms.set_generation_total_sensor(&gen_total);
  bms.set_charging_power_sensor(&power);

  bms.on_ampinvt_modbus_data(AMPINVT_STATUS_CHARGING_FRAME);

  EXPECT_NEAR(pv.state, 34.0f, 0.01f);
  EXPECT_NEAR(bat.state, 13.71f, 0.01f);
  EXPECT_NEAR(curr.state, 29.47f, 0.01f);
  EXPECT_NEAR(mppt_temp.state, 28.4f, 0.1f);
  EXPECT_NEAR(bat_temp.state, 22.6f, 0.1f);
  EXPECT_NEAR(today.state, 831.0f, 0.1f);
  EXPECT_NEAR(gen_total.state, 291.615f, 0.001f);
  EXPECT_NEAR(power.state, 404.03f, 1.0f);
}

TEST(AmpinvtStatusTest, OperationStatusFlags) {
  TestableAmpinvt bms;
  binary_sensor::BinarySensor operating, battery, fan, overheat, dc_out, t1, t2, ext_t;
  bms.set_operating_status_binary_sensor(&operating);
  bms.set_battery_status_binary_sensor(&battery);
  bms.set_fan_status_binary_sensor(&fan);
  bms.set_overheat_status_binary_sensor(&overheat);
  bms.set_dc_output_status_binary_sensor(&dc_out);
  bms.set_internal_temperature1_status_binary_sensor(&t1);
  bms.set_internal_temperature2_status_binary_sensor(&t2);
  bms.set_external_temperature_status_binary_sensor(&ext_t);

  bms.on_ampinvt_modbus_data(AMPINVT_STATUS_CHARGING_FRAME);

  EXPECT_FALSE(operating.state);
  EXPECT_FALSE(battery.state);
  EXPECT_FALSE(fan.state);
  EXPECT_FALSE(overheat.state);
  EXPECT_FALSE(dc_out.state);
  EXPECT_FALSE(t1.state);
  EXPECT_FALSE(t2.state);
  EXPECT_FALSE(ext_t.state);
}

TEST(AmpinvtStatusTest, ChargingStatusFlags) {
  TestableAmpinvt bms;
  binary_sensor::BinarySensor charging, equal, mppt, float_chg, curr_limit, derating, prohibit, pv_ov;
  bms.set_charging_status_binary_sensor(&charging);
  bms.set_equal_charging_status_binary_sensor(&equal);
  bms.set_mppt_tracking_status_binary_sensor(&mppt);
  bms.set_float_charging_status_binary_sensor(&float_chg);
  bms.set_charge_current_limit_status_binary_sensor(&curr_limit);
  bms.set_charge_derating_status_binary_sensor(&derating);
  bms.set_remote_prohibit_charging_status_binary_sensor(&prohibit);
  bms.set_pv_overvoltage_status_binary_sensor(&pv_ov);

  bms.on_ampinvt_modbus_data(AMPINVT_STATUS_CHARGING_FRAME);

  EXPECT_TRUE(charging.state);
  EXPECT_FALSE(equal.state);
  EXPECT_TRUE(mppt.state);
  EXPECT_TRUE(float_chg.state);
  EXPECT_FALSE(curr_limit.state);
  EXPECT_FALSE(derating.state);
  EXPECT_FALSE(prohibit.state);
  EXPECT_FALSE(pv_ov.state);
}

TEST(AmpinvtStatusTest, ControlStatusFlags) {
  TestableAmpinvt bms;
  binary_sensor::BinarySensor relay, load, fan_relay, overcharge, overvoltage;
  bms.set_charge_output_relay_status_binary_sensor(&relay);
  bms.set_load_output_status_binary_sensor(&load);
  bms.set_fan_relay_status_binary_sensor(&fan_relay);
  bms.set_overcharge_protection_status_binary_sensor(&overcharge);
  bms.set_overvoltage_protection_status_binary_sensor(&overvoltage);

  bms.on_ampinvt_modbus_data(AMPINVT_STATUS_CHARGING_FRAME);

  EXPECT_FALSE(relay.state);
  EXPECT_FALSE(load.state);
  EXPECT_TRUE(fan_relay.state);
  EXPECT_FALSE(overcharge.state);
  EXPECT_FALSE(overvoltage.state);
}

TEST(AmpinvtStatusTest, OperationText) {
  TestableAmpinvt bms;
  text_sensor::TextSensor text;
  bms.set_operation_status_text_sensor(&text);

  bms.on_ampinvt_modbus_data(AMPINVT_STATUS_CHARGING_FRAME);

  EXPECT_EQ(text.state, "Charging");
}

TEST(AmpinvtStatusTest, OnlineStatus) {
  TestableAmpinvt bms;
  binary_sensor::BinarySensor online;
  bms.set_online_status_binary_sensor(&online);

  bms.on_ampinvt_modbus_data(AMPINVT_STATUS_CHARGING_FRAME);

  EXPECT_TRUE(online.state);
}

// ── Anenji A3 status frame ────────────────────────────────────────────────────

TEST(AnejiStatusTest, AnalogSensors) {
  TestableAmpinvt bms;
  sensor::Sensor pv, bat, curr, mppt_temp, bat_temp, power;
  bms.set_pv_voltage_sensor(&pv);
  bms.set_battery_voltage_sensor(&bat);
  bms.set_charge_current_sensor(&curr);
  bms.set_mppt_temperature_sensor(&mppt_temp);
  bms.set_battery_temperature_sensor(&bat_temp);
  bms.set_charging_power_sensor(&power);

  bms.on_ampinvt_modbus_data(ANENJI_STATUS_FRAME);

  EXPECT_NEAR(pv.state, 23.5f, 0.01f);
  EXPECT_NEAR(bat.state, 12.59f, 0.01f);
  EXPECT_NEAR(curr.state, 7.76f, 0.01f);
  EXPECT_NEAR(mppt_temp.state, 23.4f, 0.1f);
  EXPECT_NEAR(bat_temp.state, 22.7f, 0.1f);
  EXPECT_NEAR(power.state, 97.70f, 1.0f);
}

TEST(AnejiStatusTest, ChargingStatusFlags) {
  TestableAmpinvt bms;
  binary_sensor::BinarySensor charging, equal, mppt;
  bms.set_charging_status_binary_sensor(&charging);
  bms.set_equal_charging_status_binary_sensor(&equal);
  bms.set_mppt_tracking_status_binary_sensor(&mppt);

  bms.on_ampinvt_modbus_data(ANENJI_STATUS_FRAME);

  EXPECT_TRUE(charging.state);
  EXPECT_TRUE(equal.state);
  EXPECT_TRUE(mppt.state);
}

TEST(AnejiStatusTest, OperationText) {
  TestableAmpinvt bms;
  text_sensor::TextSensor text;
  bms.set_operation_status_text_sensor(&text);

  bms.on_ampinvt_modbus_data(ANENJI_STATUS_FRAME);

  EXPECT_EQ(text.state, "Charging");
}

// ── Anenji A2 settings frame ──────────────────────────────────────────────────

TEST(AnejiSettingsTest, RatedVoltage) {
  TestableAmpinvt bms;
  sensor::Sensor rated;
  bms.set_rated_voltage_sensor(&rated);

  bms.on_ampinvt_modbus_data(ANENJI_SETTINGS_FRAME);

  EXPECT_NEAR(rated.state, 48.00f, 0.01f);
}

TEST(AnejiSettingsTest, MaxChargeCurrentLimit) {
  TestableAmpinvt bms;
  sensor::Sensor max_curr;
  bms.set_max_charge_current_limit_sensor(&max_curr);

  bms.on_ampinvt_modbus_data(ANENJI_SETTINGS_FRAME);

  EXPECT_NEAR(max_curr.state, 100.00f, 0.01f);
}

// ── Null sensors do not crash ─────────────────────────────────────────────────

TEST(AmpinvtSafetyTest, NullSensorsDoNotCrash) {
  TestableAmpinvt bms;

  EXPECT_NO_FATAL_FAILURE(bms.on_ampinvt_modbus_data(AMPINVT_STATUS_CHARGING_FRAME));
  EXPECT_NO_FATAL_FAILURE(bms.on_ampinvt_modbus_data(ANENJI_STATUS_FRAME));
  EXPECT_NO_FATAL_FAILURE(bms.on_ampinvt_modbus_data(ANENJI_SETTINGS_FRAME));
}

TEST(AmpinvtPublishDeviceUnavailableTest, InvalidatesBinarySensors) {
  TestableAmpinvt bms;
  binary_sensor::BinarySensor online, operating_status, battery_status, fan_status, overheat_status, dc_output_status;
  binary_sensor::BinarySensor internal_temperature1_status, internal_temperature2_status, external_temperature_status;
  binary_sensor::BinarySensor charging_status, equal_charging_status, mppt_tracking_status, float_charging_status;
  binary_sensor::BinarySensor charge_current_limit_status, charge_derating_status, remote_prohibit_charging_status;
  binary_sensor::BinarySensor pv_overvoltage_status, charge_output_relay_status, load_output_status, fan_relay_status;
  binary_sensor::BinarySensor overcharge_protection_status, overvoltage_protection_status;
  bms.set_online_status_binary_sensor(&online);
  bms.set_operating_status_binary_sensor(&operating_status);
  bms.set_battery_status_binary_sensor(&battery_status);
  bms.set_fan_status_binary_sensor(&fan_status);
  bms.set_overheat_status_binary_sensor(&overheat_status);
  bms.set_dc_output_status_binary_sensor(&dc_output_status);
  bms.set_internal_temperature1_status_binary_sensor(&internal_temperature1_status);
  bms.set_internal_temperature2_status_binary_sensor(&internal_temperature2_status);
  bms.set_external_temperature_status_binary_sensor(&external_temperature_status);
  bms.set_charging_status_binary_sensor(&charging_status);
  bms.set_equal_charging_status_binary_sensor(&equal_charging_status);
  bms.set_mppt_tracking_status_binary_sensor(&mppt_tracking_status);
  bms.set_float_charging_status_binary_sensor(&float_charging_status);
  bms.set_charge_current_limit_status_binary_sensor(&charge_current_limit_status);
  bms.set_charge_derating_status_binary_sensor(&charge_derating_status);
  bms.set_remote_prohibit_charging_status_binary_sensor(&remote_prohibit_charging_status);
  bms.set_pv_overvoltage_status_binary_sensor(&pv_overvoltage_status);
  bms.set_charge_output_relay_status_binary_sensor(&charge_output_relay_status);
  bms.set_load_output_status_binary_sensor(&load_output_status);
  bms.set_fan_relay_status_binary_sensor(&fan_relay_status);
  bms.set_overcharge_protection_status_binary_sensor(&overcharge_protection_status);
  bms.set_overvoltage_protection_status_binary_sensor(&overvoltage_protection_status);
  const std::vector<binary_sensor::BinarySensor *> binary_sensors = {&operating_status,
                                                                     &battery_status,
                                                                     &fan_status,
                                                                     &overheat_status,
                                                                     &dc_output_status,
                                                                     &internal_temperature1_status,
                                                                     &internal_temperature2_status,
                                                                     &external_temperature_status,
                                                                     &charging_status,
                                                                     &equal_charging_status,
                                                                     &mppt_tracking_status,
                                                                     &float_charging_status,
                                                                     &charge_current_limit_status,
                                                                     &charge_derating_status,
                                                                     &remote_prohibit_charging_status,
                                                                     &pv_overvoltage_status,
                                                                     &charge_output_relay_status,
                                                                     &load_output_status,
                                                                     &fan_relay_status,
                                                                     &overcharge_protection_status,
                                                                     &overvoltage_protection_status};

  online.publish_state(true);
  for (auto *binary_sensor : binary_sensors) {
    binary_sensor->publish_state(true);
  }

  bms.publish_device_unavailable_();

  EXPECT_TRUE(online.has_state());
  EXPECT_FALSE(online.state);
  for (size_t i = 0; i < binary_sensors.size(); i++) {
    EXPECT_FALSE(binary_sensors[i]->has_state()) << "binary sensor index " << i;
  }
}

}  // namespace esphome::ampinvt::testing
