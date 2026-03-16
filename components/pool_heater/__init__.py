import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

pool_heater_ns = cg.esphome_ns.namespace('pool_heater')
PoolHeaterComponent = pool_heater_ns.class_('PoolHeater', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(PoolHeaterComponent),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    # The magical (and now syntax-error-free) library injection
    cg.add_library("pool-heater-core=https://github.com/The-sultan/pool-heater-core.git", None)
    
    # We let ESPHome handle the C++ includes automatically!
    
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)