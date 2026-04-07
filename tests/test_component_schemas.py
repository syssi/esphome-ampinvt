"""Schema structure tests for ampinvt ESPHome component modules."""

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

import components.ampinvt as hub  # noqa: E402
from components.ampinvt import binary_sensor, sensor, text_sensor  # noqa: E402


class TestHubConstants:
    def test_conf_ampinvt_id_defined(self):
        assert hub.CONF_AMPINVT_ID == "ampinvt_id"

    def test_conf_max_no_response_count_defined(self):
        assert hub.CONF_MAX_NO_RESPONSE_COUNT == "max_no_response_count"


class TestSensorDefs:
    def test_sensor_defs_completeness(self):
        assert sensor.CONF_PV_VOLTAGE in sensor.SENSOR_DEFS
        assert sensor.CONF_CHARGE_CURRENT in sensor.SENSOR_DEFS
        assert sensor.CONF_MPPT_TEMPERATURE in sensor.SENSOR_DEFS
        assert sensor.CONF_BATTERY_TEMPERATURE in sensor.SENSOR_DEFS
        assert sensor.CONF_TODAY_YIELD in sensor.SENSOR_DEFS
        assert sensor.CONF_GENERATION_TOTAL in sensor.SENSOR_DEFS
        assert sensor.CONF_RATED_VOLTAGE in sensor.SENSOR_DEFS
        assert sensor.CONF_MAX_CHARGE_CURRENT_LIMIT in sensor.SENSOR_DEFS
        assert len(sensor.SENSOR_DEFS) == 9

    def test_battery_voltage_present(self):
        from esphome.const import CONF_BATTERY_VOLTAGE

        assert CONF_BATTERY_VOLTAGE in sensor.SENSOR_DEFS


class TestBinarySensorDefs:
    def test_binary_sensor_defs_completeness(self):
        assert binary_sensor.CONF_CHARGING_STATUS in binary_sensor.BINARY_SENSOR_DEFS
        assert binary_sensor.CONF_ONLINE_STATUS in binary_sensor.BINARY_SENSOR_DEFS
        assert binary_sensor.CONF_FAN_STATUS in binary_sensor.BINARY_SENSOR_DEFS
        assert len(binary_sensor.BINARY_SENSOR_DEFS) == 22

    def test_key_binary_sensor_names(self):
        assert binary_sensor.CONF_OPERATING_STATUS == "operating_status"
        assert binary_sensor.CONF_MPPT_TRACKING_STATUS == "mppt_tracking_status"
        assert (
            binary_sensor.CONF_OVERVOLTAGE_PROTECTION_STATUS
            == "overvoltage_protection_status"
        )


class TestTextSensors:
    def test_text_sensors_list(self):
        assert text_sensor.CONF_OPERATION_STATUS in text_sensor.TEXT_SENSORS
        assert len(text_sensor.TEXT_SENSORS) == 1
