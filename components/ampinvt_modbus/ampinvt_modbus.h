#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace ampinvt_modbus {

class AmpinvtModbusDevice;

class AmpinvtModbus : public uart::UARTDevice, public Component {
 public:
  AmpinvtModbus() = default;

  void setup() override;

  void loop() override;

  void dump_config() override;

  void register_device(AmpinvtModbusDevice *device) { this->devices_.push_back(device); }

  float get_setup_priority() const override;

  void send(uint8_t address, uint8_t function, uint16_t start_address, uint16_t register_count);

  void set_flow_control_pin(GPIOPin *flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }

 protected:
  GPIOPin *flow_control_pin_{nullptr};

  bool parse_ampinvt_modbus_byte_(uint8_t byte);

  std::vector<uint8_t> rx_buffer_;
  uint32_t last_ampinvt_modbus_byte_{0};
  std::vector<AmpinvtModbusDevice *> devices_;
};

class AmpinvtModbusDevice {
 public:
  void set_parent(AmpinvtModbus *parent) { parent_ = parent; }
  void set_address(uint8_t address) { address_ = address; }
  virtual void on_ampinvt_modbus_data(const uint8_t &function, const std::vector<uint8_t> &data) = 0;

  void send(uint8_t function, uint16_t start_address, uint16_t register_count) {
    this->parent_->send(this->address_, function, start_address, register_count);
  }

 protected:
  friend AmpinvtModbus;

  AmpinvtModbus *parent_;
  uint8_t address_;
};

}  // namespace ampinvt_modbus
}  // namespace esphome
