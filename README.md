# esphome-ampinvt

![GitHub actions](https://github.com/syssi/esphome-ampinvt/actions/workflows/ci.yaml/badge.svg)
![GitHub stars](https://img.shields.io/github/stars/syssi/esphome-ampinvt)
![GitHub forks](https://img.shields.io/github/forks/syssi/esphome-ampinvt)
![GitHub watchers](https://img.shields.io/github/watchers/syssi/esphome-ampinvt)
[!["Buy Me A Coffee"](https://img.shields.io/badge/buy%20me%20a%20coffee-donate-yellow.svg)](https://www.buymeacoffee.com/syssi)

ESPHome component to monitor and control an Ampinvt MPPT solar controller via RS485

## Supported devices

* AP-80A

## Requirements

* [ESPHome 2024.6.0 or higher](https://github.com/esphome/esphome/releases).
* ESP32 or ESP8266 board
* RS485 to TTL module (recommended: HW-0519 with auto flow control)

## Schematics

```
┌────────────────┐                ┌─────────────────────┐
│                │<----- A+ ----->│ A+                  │
│   Ampinvt      │<----- B- ----->│ B-                  │<-- 3.3V
│   MPPT         │<----- GND ---->│ GND                 │<-- GND
│   Controller   │                │       RS485-to-TTL  │<-- TX (GPIO16)
│                │                │                     │<-- RX (GPIO17)
└────────────────┘                └─────────────────────┘
```

| ESP32 Pin | HW-0519 Pin | Description      |
|-----------|-------------|------------------|
| GPIO17    | TX          | UART Transmit    |
| GPIO16    | RX          | UART Receive     |
| 3.3V      | VCC         | Power supply     |
| GND       | GND         | Ground           |

| HW-0519 Pin | Ampinvt RJ45 Pin | Description |
|-------------|------------------|-------------|
| A+          | Pin 1            | RS485 A+    |
| B-          | Pin 2            | RS485 B-    |
| GND         | Pin 8            | Ground      |

**Warning**: Pin 7 on the Ampinvt RJ45 connector carries 5V. Do not connect it to your ESP32.

## Installation

You can install this component with [ESPHome external components feature](https://esphome.io/components/external_components.html) like this:
```yaml
external_components:
  - source: github://syssi/esphome-ampinvt@main
```

or use the `esp32-example.yaml` as proof of concept:

```bash
# Install esphome
pip3 install esphome

# Clone this external component
git clone https://github.com/syssi/esphome-ampinvt.git
cd esphome-ampinvt

# Create a secrets.yaml containing some setup specific secrets
cat > secrets.yaml <<EOF
wifi_ssid: MY_WIFI_SSID
wifi_password: MY_WIFI_PASSWORD

mqtt_host: MY_MQTT_HOST
mqtt_username: MY_MQTT_USERNAME
mqtt_password: MY_MQTT_PASSWORD
EOF

# Validate the configuration, create a binary, upload it, and start logs
# If you use a esp8266 run the esp8266-example.yaml
esphome run esp32-example.yaml

```

## Example response all sensors enabled

```
[D][binary_sensor:036]: 'Online Status': Sending state ON
[I][ampinvt:067]: Status frame (37 bytes) received
[D][ampinvt:068]:   01.B3.01.00.00.00.00.01.05.27.00.00.00.E0.00.00.00.E2.00.00.00.00.02.AB.00.04.6F.E0.00.00.00.00.00.00.00.00.A4 (37)
[D][binary_sensor:036]: 'Operating Status': Sending state OFF
[D][binary_sensor:036]: 'Battery Status': Sending state OFF
[D][binary_sensor:036]: 'Fan Status': Sending state OFF
[D][binary_sensor:036]: 'Overheat Status': Sending state OFF
[D][binary_sensor:036]: 'DC Output Status': Sending state OFF
[D][binary_sensor:036]: 'Internal Temperature 1 Status': Sending state OFF
[D][binary_sensor:036]: 'Internal Temperature 2 Status': Sending state OFF
[D][binary_sensor:036]: 'External Temperature Status': Sending state OFF
[D][binary_sensor:036]: 'Charging Status': Sending state OFF
[D][binary_sensor:036]: 'Equal Charging Status': Sending state OFF
[D][binary_sensor:036]: 'MPPT Tracking Status': Sending state OFF
[D][binary_sensor:036]: 'Float Charging Status': Sending state OFF
[D][binary_sensor:036]: 'Charge Current Limit Status': Sending state OFF
[D][binary_sensor:036]: 'Charge Derating Status': Sending state OFF
[D][binary_sensor:036]: 'Remote Prohibit Charging Status': Sending state OFF
[D][binary_sensor:036]: 'PV Overvoltage Status': Sending state OFF
[D][binary_sensor:036]: 'Charge Output Relay Status': Sending state OFF
[D][binary_sensor:036]: 'Load Output Status': Sending state OFF
[D][binary_sensor:036]: 'Fan Relay Status': Sending state OFF
[D][binary_sensor:036]: 'Overcharge Protection Status': Sending state OFF
[D][binary_sensor:036]: 'Overvoltage Protection Status': Sending state OFF
[D][sensor:098]: 'PV Voltage': Sending state 0.10000 V with 1 decimals of accuracy
[D][sensor:098]: 'Battery Voltage': Sending state 13.19000 V with 2 decimals of accuracy
[D][sensor:098]: 'Charge Current': Sending state 0.00000 A with 2 decimals of accuracy
[D][sensor:098]: 'MPPT Temperature': Sending state 22.40000 °C with 1 decimals of accuracy
[D][sensor:098]: 'Battery Temperature': Sending state 22.60000 °C with 1 decimals of accuracy
[D][sensor:098]: 'Today Yield': Sending state 683.00000 W with 0 decimals of accuracy
[D][sensor:098]: 'Total Generation': Sending state 290.78403 kWh with 3 decimals of accuracy
[D][text_sensor:069]: 'Operation Status': Sending state 'Not Charging'

[I][ampinvt:144]: Settings frame (64 bytes) received
[D][ampinvt:145]:   01.B2.01.03.01.01.00.01.04.04.B0.05.7D.05.7D.04.A6.17.70.17.70.17.70.01.01.04.B0.05.AF.05.A5.00.0A.00.12.00.01.00.01.01.08.00.00.00.01.00.00.00.05.00.00.00.08.04.00.00.00.00.00.00.00.00.00.02 (64)
[I][ampinvt:152]: Settings frame decoder not implemented yet.
```

## Protocol

The Ampinvt controllers use a custom protocol similar to Modbus:

| Function | Command | Address | Data Length | Description       |
|----------|---------|---------|-------------|-------------------|
| Status   | 0xB3    | Device  | 35-36 bytes | Device status     |
| Settings | 0xB2    | Device  | Variable    | Device settings   |

## Known issues

None.

## Debugging

If this component doesn't work out of the box for your device please update your configuration to enable the debug output of the UART component and increase the log level to the see outgoing and incoming serial traffic:

```yaml
logger:
  level: DEBUG

uart:
  id: uart_0
  baud_rate: 9600
  tx_pin: GPIO16
  rx_pin: GPIO17
  debug:
    direction: BOTH
```

## References

* https://github.com/TripitakaBC/ampinvt_esphome
