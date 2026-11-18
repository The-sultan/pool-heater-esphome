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

    // Driver configuration (set from YAML via code generation)
    void set_tx_pin(uint8_t pin) { tx_pin_ = pin; }
    void set_rx_pin(uint8_t pin) { rx_pin_ = pin; }
    void set_open_drain_tx(bool open_drain) { open_drain_tx_ = open_drain; }
    void set_invert_tx(bool invert) { invert_tx_ = invert; }
    void set_invert_rx(bool invert) { invert_rx_ = invert; }
    void set_rmt_tx_channel(uint8_t channel) { rmt_tx_channel_ = channel; }  // ESP32 only
    void set_rmt_rx_channel(uint8_t channel) { rmt_rx_channel_ = channel; }  // ESP32 only

    void setup() override;
    void loop() override;
    void dump_config() override;

private:
    uint8_t tx_pin_{0};
    uint8_t rx_pin_{0};
    bool open_drain_tx_{true};
    bool invert_tx_{false};
    bool invert_rx_{false};
    uint8_t rmt_tx_channel_{0};
    uint8_t rmt_rx_channel_{2};
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