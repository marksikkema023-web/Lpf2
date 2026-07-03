#include "Lpf2/Local/ColorDistanceUartAdapter.hpp"
#include "Lpf2/Local/SerialDef.hpp"

namespace Lpf2::Local::ColorDistanceUartAdapter
{
    std::array<uint8_t, 3> buildDirectSelectFrame(uint8_t mode)
    {
        const uint8_t header = MESSAGE_CMD | LENGTH_1 | CMD_SELECT;
        return {
            header,
            mode,
            static_cast<uint8_t>(header ^ 0xFF ^ mode),
        };
    }

    uint8_t remapIncomingMode(
        DeviceType deviceType,
        uint8_t selectedMode,
        uint8_t rxModeNibble,
        uint8_t decodedMode,
        uint8_t msgLength)
    {
        (void)deviceType;
        (void)selectedMode;
        (void)rxModeNibble;
        (void)msgLength;
        return decodedMode;
    }

    bool updateExtBankFromCmd(const std::vector<uint8_t> &data, bool currentExtBank)
    {
        if (data.empty())
        {
            return currentExtBank;
        }
        return data[0] != 0;
    }
}
