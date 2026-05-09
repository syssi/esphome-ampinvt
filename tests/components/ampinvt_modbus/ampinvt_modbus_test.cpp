#include <gtest/gtest.h>
#include "common.h"

namespace esphome::ampinvt_modbus::testing {

TEST(AmpinvtModbusTest, StatusFrameDispatchedToDevice) {
  TestableAmpinvtModbus modbus;
  MockAmpinvtModbusDevice device;
  device.set_address(0x01);
  modbus.register_device(&device);

  modbus.feed(AMPINVT_STATUS_FRAME);

  EXPECT_EQ(device.call_count, 1);
  EXPECT_EQ(device.received_data, AMPINVT_STATUS_FRAME);
}

TEST(AmpinvtModbusTest, AnenjiStatusFrameDispatchedToDevice) {
  TestableAmpinvtModbus modbus;
  MockAmpinvtModbusDevice device;
  device.set_address(0x01);
  modbus.register_device(&device);

  modbus.feed(ANENJI_STATUS_FRAME);

  EXPECT_EQ(device.call_count, 1);
  EXPECT_EQ(device.received_data, ANENJI_STATUS_FRAME);
}

TEST(AmpinvtModbusTest, AnenjiSettingsFrameDispatchedToDevice) {
  TestableAmpinvtModbus modbus;
  MockAmpinvtModbusDevice device;
  device.set_address(0x01);
  modbus.register_device(&device);

  modbus.feed(ANENJI_SETTINGS_FRAME);

  EXPECT_EQ(device.call_count, 1);
}

TEST(AmpinvtModbusTest, TwoFramesDispatchedTwice) {
  TestableAmpinvtModbus modbus;
  MockAmpinvtModbusDevice device;
  device.set_address(0x01);
  modbus.register_device(&device);

  modbus.feed(ANENJI_STATUS_FRAME);
  modbus.feed(ANENJI_STATUS_FRAME);

  EXPECT_EQ(device.call_count, 2);
}

TEST(AmpinvtModbusTest, BadChecksumRejected) {
  TestableAmpinvtModbus modbus;
  MockAmpinvtModbusDevice device;
  device.set_address(0x01);
  modbus.register_device(&device);

  std::vector<uint8_t> bad_frame = ANENJI_STATUS_FRAME;
  bad_frame.back() ^= 0xFF;  // corrupt checksum
  modbus.feed(bad_frame);

  EXPECT_EQ(device.call_count, 0);
}

TEST(AmpinvtModbusTest, UnknownCommandRejected) {
  TestableAmpinvtModbus modbus;
  MockAmpinvtModbusDevice device;
  device.set_address(0x01);
  modbus.register_device(&device);

  // addr=0x01, unknown cmd=0xFF
  modbus.parse_ampinvt_modbus_byte_(0x01);
  bool result = modbus.parse_ampinvt_modbus_byte_(0xFF);
  EXPECT_FALSE(result);
  EXPECT_EQ(device.call_count, 0);
}

TEST(AmpinvtModbusTest, UnknownAddressNotDispatched) {
  TestableAmpinvtModbus modbus;
  MockAmpinvtModbusDevice device;
  device.set_address(0x99);
  modbus.register_device(&device);

  modbus.feed(ANENJI_STATUS_FRAME);  // address 0x01 != 0x99

  EXPECT_EQ(device.call_count, 0);
}

TEST(AmpinvtModbusTest, NoRegisteredDeviceDoesNotCrash) {
  TestableAmpinvtModbus modbus;
  modbus.feed(ANENJI_STATUS_FRAME);
}

TEST(AmpinvtModbusTest, PartialFrameDoesNotDispatch) {
  TestableAmpinvtModbus modbus;
  MockAmpinvtModbusDevice device;
  device.set_address(0x01);
  modbus.register_device(&device);

  for (size_t i = 0; i < ANENJI_STATUS_FRAME.size() - 2; i++)
    modbus.parse_ampinvt_modbus_byte_(ANENJI_STATUS_FRAME[i]);

  EXPECT_EQ(device.call_count, 0);
}

}  // namespace esphome::ampinvt_modbus::testing
