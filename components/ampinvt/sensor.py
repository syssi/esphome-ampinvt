import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_BATTERY_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_KILOWATT_HOURS,
    UNIT_VOLT,
    UNIT_WATT_HOURS,
)

from . import AMPINVT_COMPONENT_SCHEMA, CONF_AMPINVT_ID

DEPENDENCIES = ["ampinvt"]
CODEOWNERS = ["@syssi"]

CONF_PV_VOLTAGE = "pv_voltage"
CONF_CHARGE_CURRENT = "charge_current"
CONF_MPPT_TEMPERATURE = "mppt_temperature"
CONF_BATTERY_TEMPERATURE = "battery_temperature"
CONF_TODAY_YIELD = "today_yield"
CONF_GENERATION_TOTAL = "generation_total"
CONF_NOMINAL_VOLTAGE = "nominal_voltage"
CONF_MAX_CHARGE_CURRENT_LIMIT = "max_charge_current_limit"

# key: sensor_schema kwargs
SENSOR_DEFS = {
    CONF_PV_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_BATTERY_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CHARGE_CURRENT: {
        "unit_of_measurement": UNIT_AMPERE,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_MPPT_TEMPERATURE: {
        "unit_of_measurement": UNIT_CELSIUS,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_BATTERY_TEMPERATURE: {
        "unit_of_measurement": UNIT_CELSIUS,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_TODAY_YIELD: {
        "unit_of_measurement": UNIT_WATT_HOURS,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_ENERGY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_GENERATION_TOTAL: {
        "unit_of_measurement": UNIT_KILOWATT_HOURS,
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_ENERGY,
        "state_class": STATE_CLASS_TOTAL_INCREASING,
    },
    CONF_NOMINAL_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_MAX_CHARGE_CURRENT_LIMIT: {
        "unit_of_measurement": UNIT_AMPERE,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
}

CONFIG_SCHEMA = AMPINVT_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(key): sensor.sensor_schema(**kwargs)
        for key, kwargs in SENSOR_DEFS.items()
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_AMPINVT_ID])

    for key in SENSOR_DEFS:
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_sensor")(sens))
