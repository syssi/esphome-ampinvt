import esphome.codegen as cg
from esphome.components import ampinvt_modbus
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@syssi"]

AUTO_LOAD = [
    "ampinvt_modbus",
    "binary_sensor",
    "sensor",
    "text_sensor",
]
MULTI_CONF = True

CONF_AMPINVT_ID = "ampinvt_id"
CONF_MAX_NO_RESPONSE_COUNT = "max_no_response_count"

DEFAULT_MAX_NO_RESPONSE_COUNT = 15

ampinvt_ns = cg.esphome_ns.namespace("ampinvt")
Ampinvt = ampinvt_ns.class_(
    "Ampinvt", cg.PollingComponent, ampinvt_modbus.AmpinvtModbusDevice
)

AMPINVT_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_AMPINVT_ID): cv.use_id(Ampinvt),
    }
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Ampinvt),
            cv.Optional(
                CONF_MAX_NO_RESPONSE_COUNT, default=DEFAULT_MAX_NO_RESPONSE_COUNT
            ): cv.int_range(min=0),
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(ampinvt_modbus.ampinvt_modbus_device_schema(0x01))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ampinvt_modbus.register_ampinvt_modbus_device(var, config)

    cg.add(var.set_max_no_response_count(config[CONF_MAX_NO_RESPONSE_COUNT]))
