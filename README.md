This ESPHome wrapper is for using the pool-heater-core library (https://github.com/The-sultan/pool-heater-core) that controls pool heat pumps with board CC120A-U6.3. Thanks hkiam for his tremendously useful code (https://github.com/hkiam/pool-heat-pump/tree/main/PWP). 
To use this you need an ESP8266 (there's also code for ESP32, still not tested) connected to the heat pump through the GND and Data cables using a level shifter. After this, you just need to add to your ESPHome yaml, the following configuration:

```
external_components:
  - source: github://The-sultan/pool-heater-esphome
    components: [ pool_heater ]
pool_heater:
  id: my_pool_heater
climate:
  - platform: pool_heater
    pool_heater_id: my_pool_heater
    name: "Pool Heater Thermostat"
```
