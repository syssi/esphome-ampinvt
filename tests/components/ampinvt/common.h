#pragma once
#include "esphome/components/ampinvt/ampinvt.h"

namespace esphome::ampinvt::testing {

class TestableAmpinvt : public Ampinvt {
 public:
  void update() override {}
  void send(uint8_t function, uint16_t start_address, uint16_t register_count) override {}
  using Ampinvt::publish_device_unavailable_;
};

}  // namespace esphome::ampinvt::testing
