#pragma once

#include <array>
#include <vector>

#include "Lpf2/LWPConst.hpp"

namespace Lpf2::Local::ColorDistanceUartAdapter
{
    // Build CMD_SELECT frame for ColorDistanceSensor using full mode value.
    std::array<uint8_t, 3> buildDirectSelectFrame(uint8_t mode);

    // Keep current behavior: when selected mode is 8, some CDS frames still arrive as mode nibble 0.
    uint8_t remapIncomingMode(
        DeviceType deviceType,
        uint8_t selectedMode,
        uint8_t rxModeNibble,
        uint8_t decodedMode,
        uint8_t msgLength);

    // Update ext-bank state from CMD_EXT_MODE payload.
    bool updateExtBankFromCmd(const std::vector<uint8_t> &data, bool currentExtBank);
}
