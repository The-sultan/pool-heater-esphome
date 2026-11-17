import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.const import CONF_ID, CONF_RX_PIN, CONF_TX_PIN

pool_heater_ns = cg.esphome_ns.namespace('pool_heater')
PoolHeaterComponent = pool_heater_ns.class_('PoolHeater', cg.Component)

CONF_OPEN_DRAIN_TX = "open_drain_tx"
CONF_INVERT_TX = "invert_tx"
CONF_INVERT_RX = "invert_rx"
CONF_RMT_TX_CHANNEL = "rmt_tx_channel"
CONF_RMT_RX_CHANNEL = "rmt_rx_channel"

# Defaults match a non-inverting BSS138 bidirectional level-shifter module.
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(PoolHeaterComponent),
    cv.Required(CONF_TX_PIN): pins.internal_gpio_output_pin_number,
    cv.Required(CONF_RX_PIN): pins.internal_gpio_input_pin_number,
    cv.Optional(CONF_OPEN_DRAIN_TX, default=True): cv.boolean,
    cv.Optional(CONF_INVERT_TX, default=False): cv.boolean,
    cv.Optional(CONF_INVERT_RX, default=True): cv.boolean,
    # ESP32 only (rejected on other platforms). No schema default here so that
    # ESP8266 configs validate; the C++ defaults are TX=0 / RX=2.
    # On ESP32-S3 the RX channel must be 4-7 (e.g. rmt_rx_channel: 4).
    cv.Optional(CONF_RMT_TX_CHANNEL): cv.All(cv.only_on_esp32, cv.int_range(min=0, max=7)),
    cv.Optional(CONF_RMT_RX_CHANNEL): cv.All(cv.only_on_esp32, cv.int_range(min=0, max=7)),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    cg.add_library("pool-heater-core=https://github.com/The-sultan/pool-heater-core.git", None)

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_tx_pin(config[CONF_TX_PIN]))
    cg.add(var.set_rx_pin(config[CONF_RX_PIN]))
    cg.add(var.set_open_drain_tx(config[CONF_OPEN_DRAIN_TX]))
    cg.add(var.set_invert_tx(config[CONF_INVERT_TX]))
    cg.add(var.set_invert_rx(config[CONF_INVERT_RX]))
    if CONF_RMT_TX_CHANNEL in config:
        cg.add(var.set_rmt_tx_channel(config[CONF_RMT_TX_CHANNEL]))
    if CONF_RMT_RX_CHANNEL in config:
        cg.add(var.set_rmt_rx_channel(config[CONF_RMT_RX_CHANNEL]))
