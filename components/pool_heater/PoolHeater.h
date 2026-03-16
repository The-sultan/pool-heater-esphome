#pragma once

#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include <PoolHeaterCore.h>

// Dynamically include the correct driver based on the architecture
#ifdef USE_ESP32
  #include <ESP32HeaterDriver.h>
#elif defined(USE_ESP8266)
  #include <ESP8266HeaterDriver.h>
#endif

namespace esphome {
namespace pool_heater {

// Forward declaration
class PoolHeaterClimate;

class PoolHeater : public Component {
public:    
    // The hardware driver instance
    IHeaterHardware* driver{nullptr};
    
    // The brains of the operation!
    PoolHeaterCore* core{nullptr};

    // The linked Climate component
    PoolHeaterClimate* climate_component{nullptr};

    PoolHeater();

    void register_climate(PoolHeaterClimate* climate) { climate_component = climate; }

    void setup() override;
    void loop() override;
    void dump_config() override;
};

// --- THE CLIMATE COMPONENT ---
class PoolHeaterClimate : public climate::Climate, public Component {
private:
    PoolHeater* hub_{nullptr};

public:
    void set_hub(PoolHeater* hub) { hub_ = hub; }
    
    // Updates Home Assistant UI with the latest state from the Core
    void update_state_from_core(HeaterState state);

protected:
    // Tells Home Assistant what buttons to show
    climate::ClimateTraits traits() override;
    
    // Receives commands from Home Assistant UI
    void control(const climate::ClimateCall &call) override;
};

} // namespace pool_heater
} // namespace esphome