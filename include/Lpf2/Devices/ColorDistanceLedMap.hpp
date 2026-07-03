#pragma once

#include <stdint.h>

#include "Lpf2/Devices/ColorDistanceSensor.hpp"

namespace Lpf2::Devices::ColorDistanceLedMap
{
    uint8_t toRawMode5(ColorDistanceLedColor color);
}
