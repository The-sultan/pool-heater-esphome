#include "PoolHeater.h"

namespace esphome {
namespace pool_heater {

static const char *const TAG = "pool_heater";

PoolHeater::PoolHeater() {}

void PoolHeater::setup() {
    ESP_LOGCONFIG(TAG, "Setting up Pool Heater Custom Component...");

#ifdef USE_ESP32
    ESP_LOGI(TAG, "ESP32 detected. Initializing ESP32HeaterDriver...");
    // TX/RX = 0/0. openDrain = false, invertTx = true (BJT logic), invertRx = false
    auto* esp32_driver = new ESP32HeaterDriver(0, 0, false, true, false);
    esp32_driver->setRmtChannels(0, 2);
    driver = esp32_driver;
#elif defined(USE_ESP8266)
    ESP_LOGI(TAG, "ESP8266 detected. Initializing ESP8266HeaterDriver...");
    // TX = D2 (4), RX = D7 (13). openDrain = false, invertTx = true (BJT logic), invertRx = false
    driver = new ESP8266HeaterDriver(4, 13, false, true, false); 
#endif
    
    if (driver != nullptr) {
        // Instantiate and begin the Core using the polymorphic driver
        core = new PoolHeaterCore(driver);
        core->begin();
        ESP_LOGI(TAG, "Hardware core initialized successfully.");
    }
}

void PoolHeater::loop() {
    if (core == nullptr) return;

    // Advance the async state machine and process incoming frames
    if (core->loop()) {
        HeaterState state = core->getState();

        // --- NEW: Inject telemetry to ESPHome's logger ---
        ESP_LOGI(TAG, "Hardware State -> Power: %s, Mode: %d, In: %d, Out: %d, Target: %d", 
                 state.powerOn ? "ON" : "OFF", 
                 (int)state.mode, 
                 state.currentTempIn, 
                 state.currentTempOut, 
                 state.targetTemp);

        // Update the Climate UI if the entity is registered
        if (climate_component != nullptr) {
            climate_component->update_state_from_core(state);
        }
    }
}

void PoolHeater::dump_config() {
    ESP_LOGCONFIG(TAG, "Pool Heater Custom Component:");
}

// -----------------------------------------------------------------------------
// CLIMATE COMPONENT IMPLEMENTATION
// -----------------------------------------------------------------------------

climate::ClimateTraits PoolHeaterClimate::traits() {
    auto traits = climate::ClimateTraits();
    
    // Suppress the deprecation warning temporarily to ensure a clean build
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    traits.set_supports_current_temperature(true);
    #pragma GCC diagnostic pop
    
    // Buttons available in Home Assistant
    traits.add_supported_mode(climate::CLIMATE_MODE_OFF);
    traits.add_supported_mode(climate::CLIMATE_MODE_HEAT);
    traits.add_supported_mode(climate::CLIMATE_MODE_COOL);
    traits.add_supported_mode(climate::CLIMATE_MODE_AUTO);
    
    // UI Temperature constraints
    traits.set_visual_min_temperature(15.0);
    traits.set_visual_max_temperature(38.0);
    traits.set_visual_temperature_step(1.0);
    
    return traits;
}

void PoolHeaterClimate::control(const climate::ClimateCall &call) {
    if (hub_ == nullptr || hub_->core == nullptr) return;

    // 1. Handle Mode Changes
    if (call.get_mode().has_value()) {
        climate::ClimateMode esphome_mode = *call.get_mode();
        
        switch (esphome_mode) {
            case climate::CLIMATE_MODE_OFF:
                hub_->core->setMode(HeaterMode::OFF);
                break;
            case climate::CLIMATE_MODE_HEAT:
                hub_->core->setMode(HeaterMode::HEAT);
                break;
            case climate::CLIMATE_MODE_COOL:
                hub_->core->setMode(HeaterMode::COOL);
                break;
            case climate::CLIMATE_MODE_AUTO:
                hub_->core->setMode(HeaterMode::AUTO);
                break;
            default:
                break;
        }
    }

    // 2. Handle Target Temperature Changes
    if (call.get_target_temperature().has_value()) {
        float temp = *call.get_target_temperature();
        hub_->core->setTargetTemperature((int)temp);
    }
}

void PoolHeaterClimate::update_state_from_core(HeaterState state) {
    // Update temperatures
    this->current_temperature = state.currentTempIn;
    this->target_temperature = state.targetTemp;

    // Update the UI mode based on hardware state
    if (!state.powerOn) {
        this->mode = climate::CLIMATE_MODE_OFF;
    } else {
        switch (state.mode) {
            case HeaterMode::HEAT: this->mode = climate::CLIMATE_MODE_HEAT; break;
            case HeaterMode::COOL: this->mode = climate::CLIMATE_MODE_COOL; break;
            case HeaterMode::AUTO: this->mode = climate::CLIMATE_MODE_AUTO; break;
            default: this->mode = climate::CLIMATE_MODE_OFF; break;
        }
    }
    
    // Force Home Assistant to redraw the UI
    this->publish_state();
}

} // namespace pool_heater
} // namespace esphome