import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv

from . import AMPINVT_COMPONENT_SCHEMA, CONF_AMPINVT_ID

DEPENDENCIES = ["ampinvt"]

CONF_OPERATING_STATUS = "operating_status"
CONF_BATTERY_STATUS = "battery_status"
CONF_FAN_STATUS = "fan_status"
CONF_OVERHEAT_STATUS = "overheat_status"
CONF_DC_OUTPUT_STATUS = "dc_output_status"
CONF_INTERNAL_TEMPERATURE1_STATUS = "internal_temperature1_status"
CONF_INTERNAL_TEMPERATURE2_STATUS = "internal_temperature2_status"
CONF_EXTERNAL_TEMPERATURE_STATUS = "external_temperature_status"
CONF_CHARGING_STATUS = "charging_status"
CONF_EQUAL_CHARGING_STATUS = "equal_charging_status"
CONF_MPPT_TRACKING_STATUS = "mppt_tracking_status"
CONF_FLOAT_CHARGING_STATUS = "float_charging_status"
CONF_CHARGE_CURRENT_LIMIT_STATUS = "charge_current_limit_status"
CONF_CHARGE_DERATING_STATUS = "charge_derating_status"
CONF_REMOTE_PROHIBIT_CHARGING_STATUS = "remote_prohibit_charging_status"
CONF_PV_OVERVOLTAGE_STATUS = "pv_overvoltage_status"
CONF_CHARGE_OUTPUT_RELAY_STATUS = "charge_output_relay_status"
CONF_LOAD_OUTPUT_STATUS = "load_output_status"
CONF_FAN_RELAY_STATUS = "fan_relay_status"
CONF_OVERCHARGE_PROTECTION_STATUS = "overcharge_protection_status"
CONF_OVERVOLTAGE_PROTECTION_STATUS = "overvoltage_protection_status"
CONF_ONLINE_STATUS = "online_status"

BINARY_SENSORS = [
    CONF_OPERATING_STATUS,
    CONF_BATTERY_STATUS,
    CONF_FAN_STATUS,
    CONF_OVERHEAT_STATUS,
    CONF_DC_OUTPUT_STATUS,
    CONF_INTERNAL_TEMPERATURE1_STATUS,
    CONF_INTERNAL_TEMPERATURE2_STATUS,
    CONF_EXTERNAL_TEMPERATURE_STATUS,
    CONF_CHARGING_STATUS,
    CONF_EQUAL_CHARGING_STATUS,
    CONF_MPPT_TRACKING_STATUS,
    CONF_FLOAT_CHARGING_STATUS,
    CONF_CHARGE_CURRENT_LIMIT_STATUS,
    CONF_CHARGE_DERATING_STATUS,
    CONF_REMOTE_PROHIBIT_CHARGING_STATUS,
    CONF_PV_OVERVOLTAGE_STATUS,
    CONF_CHARGE_OUTPUT_RELAY_STATUS,
    CONF_LOAD_OUTPUT_STATUS,
    CONF_FAN_RELAY_STATUS,
    CONF_OVERCHARGE_PROTECTION_STATUS,
    CONF_OVERVOLTAGE_PROTECTION_STATUS,
    CONF_ONLINE_STATUS,
]

CONFIG_SCHEMA = AMPINVT_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_OPERATING_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:alert-circle"
        ),
        cv.Optional(CONF_BATTERY_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:battery-alert"
        ),
        cv.Optional(CONF_FAN_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:fan"
        ),
        cv.Optional(CONF_OVERHEAT_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:thermometer-alert"
        ),
        cv.Optional(CONF_DC_OUTPUT_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:alert-circle"
        ),
        cv.Optional(
            CONF_INTERNAL_TEMPERATURE1_STATUS
        ): binary_sensor.binary_sensor_schema(icon="mdi:thermometer"),
        cv.Optional(
            CONF_INTERNAL_TEMPERATURE2_STATUS
        ): binary_sensor.binary_sensor_schema(icon="mdi:thermometer"),
        cv.Optional(
            CONF_EXTERNAL_TEMPERATURE_STATUS
        ): binary_sensor.binary_sensor_schema(icon="mdi:thermometer"),
        cv.Optional(CONF_CHARGING_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:battery-charging"
        ),
        cv.Optional(CONF_EQUAL_CHARGING_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:battery-charging"
        ),
        cv.Optional(CONF_MPPT_TRACKING_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:chart-line"
        ),
        cv.Optional(CONF_FLOAT_CHARGING_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:battery-charging"
        ),
        cv.Optional(
            CONF_CHARGE_CURRENT_LIMIT_STATUS
        ): binary_sensor.binary_sensor_schema(icon="mdi:current-dc"),
        cv.Optional(CONF_CHARGE_DERATING_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:alert-circle"
        ),
        cv.Optional(
            CONF_REMOTE_PROHIBIT_CHARGING_STATUS
        ): binary_sensor.binary_sensor_schema(icon="mdi:remote"),
        cv.Optional(CONF_PV_OVERVOLTAGE_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:flash-alert"
        ),
        cv.Optional(
            CONF_CHARGE_OUTPUT_RELAY_STATUS
        ): binary_sensor.binary_sensor_schema(icon="mdi:electric-switch"),
        cv.Optional(CONF_LOAD_OUTPUT_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:electric-switch"
        ),
        cv.Optional(CONF_FAN_RELAY_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:fan"
        ),
        cv.Optional(
            CONF_OVERCHARGE_PROTECTION_STATUS
        ): binary_sensor.binary_sensor_schema(icon="mdi:shield-alert"),
        cv.Optional(
            CONF_OVERVOLTAGE_PROTECTION_STATUS
        ): binary_sensor.binary_sensor_schema(icon="mdi:shield-alert"),
        cv.Optional(CONF_ONLINE_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:connection"
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_AMPINVT_ID])

    for key in BINARY_SENSORS:
        if key in config:
            conf = config[key]
            sens = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
