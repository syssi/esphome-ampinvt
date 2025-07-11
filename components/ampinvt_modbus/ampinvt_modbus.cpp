#include "ampinvt_modbus.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ampinvt_modbus {

static const char *const TAG = "ampinvt_modbus";

// Ampinvt uses a custom protocol that's not standard modbus
static const uint8_t AMPINVT_FRAME_SIZE = 37;
static const uint8_t AMPINVT_COMMAND_STATUS = 0xB3;
static const uint8_t AMPINVT_COMMAND_SETTINGS = 0xB2;

void AmpinvtModbus::setup() {
  if (this->flow_control_pin_ != nullptr) {
    this->flow_control_pin_->setup();
  }
}

void AmpinvtModbus::loop() {
  const uint32_t now = millis();
  if (now - this->last_ampinvt_modbus_byte_ > 100) {
    this->rx_buffer_.clear();
    this->last_ampinvt_modbus_byte_ = now;
  }

  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (this->parse_ampinvt_modbus_byte_(byte)) {
      this->last_ampinvt_modbus_byte_ = now;
    } else {
      this->rx_buffer_.clear();
    }
  }
}

bool AmpinvtModbus::parse_ampinvt_modbus_byte_(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);

  // Ampinvt protocol expects exactly 37 bytes
  if (at < AMPINVT_FRAME_SIZE - 1) {
    return true;
  }

  if (at == AMPINVT_FRAME_SIZE - 1) {
    // We have received all 37 bytes, process the frame
    const uint8_t *raw = &this->rx_buffer_[0];
    uint8_t address = raw[0];
    uint8_t function = raw[1];

    // Pass complete frame including address and function bytes
    std::vector<uint8_t> data(this->rx_buffer_.begin(), this->rx_buffer_.end());

    bool found = false;
    for (auto *device : this->devices_) {
      if (device->address_ == address) {
        device->on_ampinvt_modbus_data(function, data);
        found = true;
      }
    }
    if (!found) {
      ESP_LOGW(TAG, "Got Ampinvt frame from unknown address 0x%02X!", address);
    }

    // return false to reset buffer
    return false;
  }

  return true;
}

void AmpinvtModbus::dump_config() {
  ESP_LOGCONFIG(TAG, "AmpinvtModbus:");
  LOG_PIN("  Flow Control Pin: ", this->flow_control_pin_);
}

float AmpinvtModbus::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}

void AmpinvtModbus::send(uint8_t address, uint8_t function, uint16_t start_address, uint16_t register_count) {
  // Ampinvt uses a fixed 8-byte command structure
  uint8_t frame[8];
  frame[0] = address;
  frame[1] = function;
  frame[2] = 0x01;  // Fixed byte
  frame[3] = 0x00;  // Fixed byte
  frame[4] = 0x00;  // Fixed byte
  frame[5] = 0x00;  // Fixed byte
  frame[6] = 0x00;  // Fixed byte

  // Calculate checksum (simple sum of all bytes)
  uint8_t checksum = 0;
  for (int i = 0; i < 7; i++) {
    checksum += frame[i];
  }
  frame[7] = checksum;

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(true);

  this->write_array(frame, 8);
  this->flush();

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(false);
}

}  // namespace ampinvt_modbus
}  // namespace esphome
