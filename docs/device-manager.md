# Device Manager & Capabilities

`DeviceManager` wraps any `Port` and automatically constructs the right typed `PortDevice` when a device connects, using the registered `DeviceFactory` instances.

## Setup

```cpp
Lpf2::DeviceRegistry::registerDefault(); // once at startup
```

## Basic usage

```cpp
Lpf2::DeviceManager dm(port); // port is any Lpf2::Port subclass

void loop()
{
    dm.update(); // calls port.update(), manages device lifecycle

    if (dm.device())
    {
        // a device is connected and constructed
    }
}
```

## Capabilities

Devices expose typed interfaces via capability IDs. IDs are compile-time FNV-1a hashes — no runtime table required.

```cpp
if (auto *motor = static_cast<Lpf2::Devices::BasicMotorControl *>(
        dm.device()->getCapability(Lpf2::Devices::BasicMotor::CAP)))
{
    motor->setSpeed(50);
}

if (auto *sensor = static_cast<Lpf2::Devices::TechnicColorSensorControl *>(
        dm.device()->getCapability(Lpf2::Devices::TechnicColorSensor::CAP)))
{
    Serial.println(sensor->getColorIdx());
}
```

`getCapability()` returns `nullptr` if the connected device does not implement that capability.

## Built-in capabilities

| Capability class | CAP constant | Devices |
| --- | --- | --- |
| `BasicMotorControl` | `BasicMotor::CAP` | All motors |
| `EncoderMotorControl` | `EncoderMotor::CAP` | Technic motors with encoder |
| `TechnicColorSensorControl` | `TechnicColorSensor::CAP` | Technic color sensor |
| `DistanceSensorControl` | `DistanceSensor::CAP` | Technic distance sensor |

`EncoderMotor` also implements `BasicMotorControl` — all motors share that capability.

## Custom device factory

```cpp
class MyFactory : public Lpf2::DeviceFactory
{
public:
    bool matches(const Lpf2::Port &port) const override
    {
        return port.getDeviceType() == Lpf2::DeviceType::MY_TYPE;
    }

    Lpf2::PortDevice *create(Lpf2::Port &port) const override
    {
        return new MyDevice(port);
    }

    const char *name() const override { return "My Device Factory"; }
};

namespace { MyFactory factory; }

// Call once at startup, before using DeviceManager
Lpf2::DeviceRegistry::instance().registerFactory(&factory);
```

See [custom-device.md](custom-device.md) for a full example with capability interfaces.
