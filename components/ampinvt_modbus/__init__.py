from esphome import pins
import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ADDRESS, CONF_FLOW_CONTROL_PIN, CONF_ID
from esphome.cpp_helpers import gpio_pin_expression

CODEOWNERS = ["@syssi"]

DEPENDENCIES = ["uart"]
MULTI_CONF = True

CONF_AMPINVT_MODBUS_ID = "ampinvt_modbus_id"

ampinvt_modbus_ns = cg.esphome_ns.namespace("ampinvt_modbus")
AmpinvtModbus = ampinvt_modbus_ns.class_("AmpinvtModbus", cg.Component, uart.UARTDevice)
AmpinvtModbusDevice = ampinvt_modbus_ns.class_("AmpinvtModbusDevice")

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(AmpinvtModbus),
            cv.Optional(CONF_FLOW_CONTROL_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    cg.add_global(ampinvt_modbus_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    await uart.register_uart_device(var, config)

    if CONF_FLOW_CONTROL_PIN in config:
        pin = await gpio_pin_expression(config[CONF_FLOW_CONTROL_PIN])
        cg.add(var.set_flow_control_pin(pin))


def ampinvt_modbus_device_schema(default_address):
    schema = {
        cv.GenerateID(CONF_AMPINVT_MODBUS_ID): cv.use_id(AmpinvtModbus),
    }
    if default_address is None:
        schema[cv.Required(CONF_ADDRESS)] = cv.hex_uint8_t
    else:
        schema[cv.Optional(CONF_ADDRESS, default=default_address)] = cv.hex_uint8_t
    return cv.Schema(schema)


async def register_ampinvt_modbus_device(var, config):
    parent = await cg.get_variable(config[CONF_AMPINVT_MODBUS_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(parent.register_device(var))
