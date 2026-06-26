#include "Lpf2/config.hpp"
#include "Lpf2/Util/Utils.hpp"

namespace Lpf2::Utils
{
    float map(float x, float in_min, float in_max, float out_min, float out_max)
    {
        if (in_max == in_min)
            return out_min;
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
}