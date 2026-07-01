#ifndef INTERNAL_SENSORS_HPP
#define INTERNAL_SENSORS_HPP

#include <Arduino.h>
#include <vector>

// Forward declaration matching your actual framework class
namespace Lpf2 {
    class HubEmulation;
//    class Hub;
}

// LED Color definition layout
struct LEDColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

// Global Initialization
void initInternalSensors(Lpf2::HubEmulation &hub);

// Background update loop for read-only sensors (Temperature, IMU, etc.)
void updateInternalSensors();

namespace Lpf2
{
    // The write callback used by your LPF2 device registration macro
    int handleLEDWrite(uint8_t mode, const std::vector<uint8_t> &data, void *userData);
}

#endif // INTERNAL_SENSORS_HPP