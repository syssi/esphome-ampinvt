import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv

from . import AMPINVT_COMPONENT_SCHEMA, CONF_AMPINVT_ID

DEPENDENCIES = ["ampinvt"]

CONF_OPERATION_STATUS = "operation_status"

TEXT_SENSORS = [
    CONF_OPERATION_STATUS,
]

CONFIG_SCHEMA = AMPINVT_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_OPERATION_STATUS): text_sensor.text_sensor_schema(
            icon="mdi:solar-power"
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_AMPINVT_ID])

    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = await text_sensor.new_text_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
