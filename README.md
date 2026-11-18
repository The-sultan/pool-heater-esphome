This ESPHome wrapper is for using the pool-heater-core library (https://github.com/The-sultan/pool-heater-core) that controls pool heat pumps with board CC120A-U6.3. Thanks hkiam for his tremendously useful code (https://github.com/hkiam/pool-heat-pump/tree/main/PWP).

To use this you need an ESP8266 (there's also code for ESP32, still not tested) connected to the heat pump through the GND and Data cables using a level shifter.

> **Breaking change:** `tx_pin` and `rx_pin` are now **required** under `pool_heater:`. Existing YAMLs must add them
> (the previously hardcoded ESP8266 values were `tx_pin: GPIO4` and `rx_pin: GPIO13`). Note that the defaults for
> `open_drain_tx` / `invert_tx` / `invert_rx` now target a BSS138 level shifter; if you use an inverting transistor
> front end, see [Inverting transistor front end](#inverting-transistor-front-end-eg-2n2222).

> **Semantic change (pool-heater-core 2.0.0):** `invert_tx` and `invert_rx` now both mean *"the front end inverts
> the signal"*, on ESP8266 and ESP32 alike. Previously the ESP8266 driver used the opposite meaning for RX, so a
> BSS138 needed `invertRx = true` there. If you relied on the old ESP8266 RX polarity (for example a local build
> with `invertRx = true` on a BSS138), flip it: BSS138 is now `invert_rx: false`, 2N2222 is now `invert_rx: true`.

## Example: D1 Mini + BSS138 level-shifter module

```yaml
external_components:
  - source: github://The-sultan/pool-heater-esphome
    components: [ pool_heater ]

esp8266:
  board: d1_mini

pool_heater:
  id: my_pool_heater
  tx_pin: GPIO4      # D2
  rx_pin: GPIO13     # D7
  # open_drain_tx / invert_tx / invert_rx use the BSS138 defaults

climate:
  - platform: pool_heater
    pool_heater_id: my_pool_heater
    name: "Pool Heater Thermostat"
```

## Configuration variables (`pool_heater:`)

| Key              | Required | Default | Description |
|------------------|----------|---------|-------------|
| `tx_pin`         | yes      | —       | GPIO used to transmit to the heat pump data line. |
| `rx_pin`         | yes      | —       | GPIO used to receive from the heat pump data line. |
| `open_drain_tx`  | no       | `true`  | Drive TX as open-drain (only pulls low, never drives high). |
| `invert_tx`      | no       | `false` | Set to `true` if the TX front end inverts the signal. |
| `invert_rx`      | no       | `false` | Set to `true` if the RX front end inverts the signal. |
| `rmt_tx_channel` | no       | `0`     | **ESP32 only.** RMT channel used for TX. |
| `rmt_rx_channel` | no       | `2`     | **ESP32 only.** RMT channel used for RX. |

The defaults match a non-inverting BSS138 bidirectional level-shifter module, which is the reference hardware.
The invert flags mean the same thing on ESP8266 and ESP32.

`rmt_tx_channel` / `rmt_rx_channel` are rejected with a config error on non-ESP32 platforms. The defaults (TX 0 / RX 2)
work on ESP32 classic, C3 and S2. On an **ESP32-S3**, channels 0-3 are TX-only and 4-7 are RX-only, so you must set
e.g. `rmt_rx_channel: 4`.

### Inverting transistor front end (e.g. 2N2222)

If your interface uses an inverting transistor stage instead of a BSS138 module, override the signal options:

```yaml
pool_heater:
  id: my_pool_heater
  tx_pin: GPIO4      # D2
  rx_pin: GPIO13     # D7
  open_drain_tx: false
  invert_tx: true
  invert_rx: true
```

### ESP32 example

```yaml
esp32:
  board: esp32dev
  framework:
    type: arduino

pool_heater:
  id: my_pool_heater
  tx_pin: GPIO17
  rx_pin: GPIO16
  # rmt_tx_channel: 0
  # rmt_rx_channel: 4   # required on ESP32-S3
```
