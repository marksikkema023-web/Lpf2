#include "Lpf2/Devices/ColorDistanceLedMap.hpp"

namespace Lpf2::Devices::ColorDistanceLedMap
{
    uint8_t toRawMode5(ColorDistanceLedColor color)
    {
        switch (color)
        {
        case ColorDistanceLedColor::BLUE:
            return 0x03;
        case ColorDistanceLedColor::GREEN:
            return 0x05;
        case ColorDistanceLedColor::YELLOW:
            // Real CDS appears to treat yellow as off in COL O mode.
            return 0x00;
        case ColorDistanceLedColor::RED:
            return 0x09;
        case ColorDistanceLedColor::WHITE:
            return 0x0A;
        case ColorDistanceLedColor::OFF:
        default:
            return 0x00;
        }
    }
}
