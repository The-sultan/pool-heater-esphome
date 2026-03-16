import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate
from esphome.const import (
    CONF_ID, 
    CONF_NAME, 
    CONF_DISABLED_BY_DEFAULT, 
    CONF_INTERNAL, 
    CONF_ICON,
    CONF_VISUAL
)

# Reference the namespace and main component from __init__.py
from . import pool_heater_ns, PoolHeaterComponent

CONF_POOL_HEATER_ID = "pool_heater_id"

# Define the Climate class inheriting from climate.Climate
PoolHeaterClimate = pool_heater_ns.class_('PoolHeaterClimate', climate.Climate, cg.Component)

# Create a pure Schema, explicitly adding the base Entity fields and the Visual dictionary
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(PoolHeaterClimate),
    cv.Required(CONF_POOL_HEATER_ID): cv.use_id(PoolHeaterComponent),
    cv.Optional(CONF_NAME): cv.string,
    
    # Injecting the missing base entity configuration defaults
    cv.Optional(CONF_DISABLED_BY_DEFAULT, default=False): cv.boolean,
    cv.Optional(CONF_INTERNAL, default=False): cv.boolean,
    cv.Optional(CONF_ICON, default=""): cv.icon,
    
    # Injecting the visual dictionary to prevent KeyError: 'visual' in ESPHome 2025.x
    cv.Optional(CONF_VISUAL, default={}): cv.Schema({
        cv.Optional("min_temperature"): cv.float_,
        cv.Optional("max_temperature"): cv.float_,
        cv.Optional("temperature_step"): cv.float_,
    }),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    
    # Register the component and the climate platform
    await cg.register_component(var, config)
    await climate.register_climate(var, config)

    # Link this climate component to the main Hub instance
    hub = await cg.get_variable(config[CONF_POOL_HEATER_ID])
    cg.add(var.set_hub(hub))
    cg.add(hub.register_climate(var))