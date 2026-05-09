# Remote Port (Hub-connected Device)

`Remote::Port` proxies a device attached to a LEGO Hub over BLE.

## Connecting to a hub

```cpp
#include "Lpf2/Hub.hpp"

Lpf2::Hub hub;

void loop()
{
    vTaskDelay(1);

    if (!hub.isConnected() && !hub.isConnecting())
    {
        hub.init();
        vTaskDelay(500);
    }

    if (hub.isConnecting())
    {
        hub.connectHub();
        if (hub.isConnected())
            Serial.println("Connected");
    }

    if (hub.isConnected())
    {
        // use hub ports here
    }
}
```

## Using a port

```cpp
// Get port A from a Technic Hub
auto &portA = *hub.getPort(Lpf2::PortNum(Lpf2::ControlPlusHubPort::A));

portA.update();

if (portA.getDeviceType() == Lpf2::DeviceType::TECHNIC_COLOR_SENSOR)
{
    portA.setMode(0);                        // call once after device connects
    Serial.println(portA.getValue(0, 0));    // color index
}

if (portA.getDeviceType() == Lpf2::DeviceType::TECHNIC_LARGE_LINEAR_MOTOR)
{
    portA.startSpeed(50, 100);
}
```

## Using DeviceManager with a remote port

```cpp
Lpf2::DeviceManager dm(portA);

dm.update();

if (dm.device())
{
    if (auto *sensor = static_cast<Lpf2::Devices::TechnicColorSensorControl *>(
            dm.device()->getCapability(Lpf2::Devices::TechnicColorSensor::CAP)))
    {
        Serial.println(sensor->getColorIdx());
    }
}
```

See [device-manager.md](device-manager.md) for full capability API.

## Hub emulation

See `HubEmulation.hpp` and the `EmulatedHub` example. Emulation uses `Virtual::Port` internally.
