#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/ampinvt_modbus/ampinvt_modbus.h"

namespace esphome::ampinvt_modbus::testing {

// Real frames (address=0x01) from the faker YAMLs.

// AMPINVT status frame: address=0x01, command=0xB3, 37 bytes
static const std::vector<uint8_t> AMPINVT_STATUS_FRAME = {
    0x01, 0xB3, 0x01, 0x00, 0x0D, 0x04, 0x01, 0x54, 0x05, 0x5B, 0x0B, 0x83, 0x01, 0x1C, 0x00, 0x00, 0x00, 0xE2, 0x00,
    0x00, 0x00, 0x00, 0x03, 0x3F, 0x00, 0x04, 0x73, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0,
};

// Anenji status frame: address=0x01, command=0xA3, 21 bytes
static const std::vector<uint8_t> ANENJI_STATUS_FRAME = {
    0x01, 0xA3, 0x01, 0x00, 0x07, 0x00, 0x00, 0xEB, 0x04, 0xEB, 0x03,
    0x08, 0x00, 0xEA, 0x00, 0x00, 0x00, 0xE3, 0x01, 0x53, 0xB2,
};

// Anenji settings frame: address=0x01, command=0xA2, 26 bytes
static const std::vector<uint8_t> ANENJI_SETTINGS_FRAME = {
    0x01, 0xA2, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0xC0, 0x00, 0x00,
    0x15, 0x40, 0x00, 0x00, 0x00, 0x00, 0x27, 0x10, 0x00, 0x00, 0x00, 0x00, 0x02,
};

class MockAmpinvtModbusDevice : public AmpinvtModbusDevice {
 public:
  std::vector<uint8_t> received_data;
  int call_count{0};

  void on_ampinvt_modbus_data(const std::vector<uint8_t> &data) override {
    received_data = data;
    call_count++;
  }
  void send(uint8_t function, uint16_t start_address, uint16_t register_count) override {}
};

class TestableAmpinvtModbus : public AmpinvtModbus {
 public:
  void loop() override {}
  using AmpinvtModbus::parse_ampinvt_modbus_byte_;

  bool feed(const std::vector<uint8_t> &frame) {
    bool result = false;
    for (uint8_t byte : frame)
      result = parse_ampinvt_modbus_byte_(byte);
    return result;
  }
};

}  // namespace esphome::ampinvt_modbus::testing
