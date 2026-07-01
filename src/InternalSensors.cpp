#include "Lpf2/log/log.h"
#include "Lpf2/HubEmulation.hpp"
#include "InternalSensors.hpp"
#include <Adafruit_NeoPixel.h>
#include <NimBLEDevice.h>

// LED Control Pins configuration
#if defined(USING_XIAO)
    #define LED_DATA_PIN 2       
#elif defined(USING_SUPERMINI)
    #define LED_DATA_PIN 48      // ESP32-S3-SuperMini built-in RGB LED on GPIO48
#else
    #define LED_DATA_PIN 2       
#endif

#define NUM_LEDS 1 
#define LED_TYPE NEO_GRB 
#define LED_BRIGHTNESS 255       // Maximum brightness (0-255)

// Static instance for the Build-in LED
static Adafruit_NeoPixel strip(NUM_LEDS, LED_DATA_PIN, LED_TYPE + NEO_KHZ800);

// Map LEGO color codes to RGB values for the Build-in LED
static const LEDColor ledColorMap[] = {
    {0, 0, 0},           // 0x00: Black (off)
    {255, 192, 203},     // 0x01: Pink
    {255, 0, 255},       // 0x02: Purple/Magenta
    {0, 0, 255},         // 0x03: Blue
    {0, 150, 255},       // 0x04: Light Blue
    {0, 255, 255},       // 0x05: Cyan
    {0, 255, 0},         // 0x06: Green
    {255, 255, 0},       // 0x07: Yellow
    {255, 60, 0},        // 0x08: Orange
    {255, 0, 0},         // 0x09: Red
    {255, 255, 255},     // 0x0A: White
};
static const int NUM_COLORS = sizeof(ledColorMap) / sizeof(LEDColor);

void initInternalSensors(Lpf2::HubEmulation &hub) {
    (void)hub;

    // Initialize the NeoPixel LED strip for hub LED color control.
    // Real ColorDistanceSensor data comes from the physical UART sensor on port A.
    strip.begin();
    strip.setBrightness(LED_BRIGHTNESS);
    strip.setPixelColor(0, strip.Color(0, 0, 0));  // Start LED off
    strip.show();    
}

void updateInternalSensors() {
    unsigned long currentMillis = millis();
    
    // Check if the iPad app is NOT connected
    auto *server = NimBLEDevice::getServer();
    if (server == nullptr || server->getConnectedCount() == 0) {
        const unsigned long cycleDuration = 1500; // 1.5 seconds per double-flash rhythm
        unsigned long currentCycleTime = currentMillis % cycleDuration;

        // Execute non-blocking double-flash logic matching the original hub
        if (currentCycleTime < 100) {
            strip.setPixelColor(0, strip.Color(127, 127, 127)); // Flash 1 (White)
        } else if (currentCycleTime < 250) {
            strip.setPixelColor(0, strip.Color(0, 0, 0));       // Gap (Off)
        } else if (currentCycleTime < 350) {
            strip.setPixelColor(0, strip.Color(127, 127, 127)); // Flash 2 (White)
        } else {
            strip.setPixelColor(0, strip.Color(0, 0, 0));       // Long Pause (Off)
        }
        strip.show();
    } 
    else {
        // Keep connection-state indicator silent when app is connected.
        // Sensor value updates are produced by the real UART device path only.
        strip.setPixelColor(0, strip.Color(0, 0, 0));
        strip.show();
    }
}

namespace Lpf2
{
    int handleLEDWrite(uint8_t mode, const std::vector<uint8_t> &data, void *userData)
    {
        if (data.empty()) return 0;
        
        switch (mode) 
        {
            case 0: // Mode 0: Indexed Color Mode
            {
                uint8_t colorCode = data[0];
                LPF2_LOG_I("[LED] Mode 0 (Indexed) command received: 0x%02X", colorCode);
                
                if (colorCode < NUM_COLORS) {
                    const LEDColor &color = ledColorMap[colorCode];
                    LPF2_LOG_I("[LED] Setting indexed color to RGB(%d, %d, %d)", color.red, color.green, color.blue);
                    
                    strip.setPixelColor(0, strip.Color(color.red, color.green, color.blue));
                    strip.show();
                } else {
                    LPF2_LOG_W("[LED] Unknown color code index: 0x%02X", colorCode);
                }
                break;
            }
            case 1: // Mode 1: Direct/Raw RGB Mode
            {
                if (data.size() < 3) {
                    LPF2_LOG_W("[LED] Mode 1 command received but payload size is insufficient: %d bytes", data.size());
                    return 0;
                }
                
                uint8_t r = data[0];
                uint8_t g = data[1];
                uint8_t b = data[2];
                
                LPF2_LOG_I("[LED] Mode 1 (Raw RGB) command received: RGB(%d, %d, %d)", r, g, b);
                
                strip.setPixelColor(0, strip.Color(r, g, b));
                strip.show();
                break;
            }
            default:
                LPF2_LOG_W("[LED] Unsupported mode requested: %d (Payload size: %d bytes)", mode, data.size());
                break;
        }
        
        return 0;
    }
}