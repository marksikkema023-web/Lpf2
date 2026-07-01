/**
 *  Copyright (C) 2026 - Rbel12b
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  */


#include <Arduino.h>

#include "Lpf2/HubEmulation.hpp"
#include "Lpf2/DeviceDescLib.hpp"
#include "Lpf2/DeviceFactory.hpp"

#include "driver/mcpwm.h"

#include "Lpf2/log/log.h"

#include "InternalSensors.hpp"

Lpf2::HubEmulation hub("Technic Hub", Lpf2::HubType::CONTROL_PLUS_HUB);

#define USE_LOCAL_PORT 1

#if defined(USING_XIAO)
// Port PWM pins (Mapped to D2 and D3)
    #define PORT_A_PWM_1 3  // XIAO D2 (GPIO 3)
    #define PORT_A_PWM_2 4  // XIAO D3 (GPIO 4)
// Port ID/UART pins (Serial1? Mapped to D0 and D1, Serial0 uses default D6/D7?)
    #define PORT_A_ID_1 1   // XIAO D0 (GPIO 1) -> External 47k Pullup to 3.3V
    #define PORT_A_ID_2 2   // XIAO D1 (GPIO 2) -> External 47k Pullup to 3.3V
#elif defined(USING_SUPERMINI) // GPIO3, this pin is dangerous for UART with external pullup, is briefly high at boot
// Port PWM pins
    #define PORT_A_PWM_1 4  // GPIO4
    #define PORT_A_PWM_2 5  // GPIO5
// Port ID/UART pins
    #define PORT_A_ID_1 6   // GPIO6 -> External 47k Pullup to 3.3V
    #define PORT_A_ID_2 7   // GPIO7 -> External 47k Pullup to 3.3V
#else
    #error "No valid target board defined! Please select an environment in PlatformIO."
#endif

#ifdef USE_LOCAL_PORT
#include "../examples/LocalPort/device.hpp"
#include "Lpf2/Local/Port.hpp"

Esp32IO io(1);
Lpf2::Local::Port port(io);

#endif

void setup()
{
    Serial.begin(981200);
    lpf2_set_runtime_log_level(LPF2_LOG_LEVEL_DEBUG); // enable debug output

    initInternalSensors(hub);

    Lpf2::DeviceRegistry::registerDefault();
    Lpf2::DeviceDescRegistry::registerDefault();
    hub.setUseBuiltInDevices(true);
    hub.start();
    hub.setBatteryLevel(80);

#ifdef USE_LOCAL_PORT
    io.init(PORT_A_ID_1, PORT_A_ID_2, PORT_A_PWM_1, PORT_A_PWM_2);
    port.init();
    hub.attachPort(Lpf2::PortNum(Lpf2::ControlPlusHubPort::A), &port);
#endif
}

void loop()
{
    vTaskDelay(1);

    if (Serial.available()) {
        uint8_t c = Serial.read();
        if (c == 0x03) {
            // Ctrl+C received
            ESP.restart();
        }
    }

#ifdef USE_LOCAL_PORT
    updateInternalSensors();
    port.update();
#endif
}
