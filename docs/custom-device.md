# Adding a Custom Device

A device needs: a capability interface, a `PortDevice` implementation, and a `DeviceFactory`.

## 1. Header

```cpp
#pragma once

#include "Lpf2/Device.hpp"
#include "Lpf2/DeviceFactory.hpp"

namespace Lpf2::Devices
{
    // Capability interface — pure virtual, so any device can implement it
    class BasicMotorControl
    {
    public:
        virtual ~BasicMotorControl() = default;
        virtual void setSpeed(int8_t speed) = 0;
    };

    class BasicMotor : public PortDevice, public BasicMotorControl
    {
    public:
        BasicMotor(Port &port) : PortDevice(port) {}

        bool init() override      { setSpeed(0); return true; }
        void poll() override      {}
        const char *name() const override { return "Basic Motor"; }

        void setSpeed(int8_t speed) override;

        bool hasCapability(DeviceCapabilityId id) const override;
        void *getCapability(DeviceCapabilityId id) override;

        inline static const DeviceCapabilityId CAP =
            Lpf2CapabilityRegistry::registerCapability("basic_motor");

        static void registerFactory(DeviceRegistry &reg);
    };

    class BasicMotorFactory : public DeviceFactory
    {
    public:
        bool matches(const Port &port) const override;
        PortDevice *create(Port &port) const override { return new BasicMotor(port); }
        const char *name() const override { return "Basic Motor Factory"; }
    };
}
```

## 2. Source

```cpp
#include "Lpf2/Devices/BasicMotor.hpp"

namespace Lpf2::Devices
{
    namespace { BasicMotorFactory factory; }

    void BasicMotor::registerFactory(DeviceRegistry &reg)
    {
        reg.registerFactory(&factory);
    }

    void BasicMotor::setSpeed(int8_t speed)
    {
        m_port.startPower(speed);
    }

    bool BasicMotor::hasCapability(DeviceCapabilityId id) const
    {
        return id == CAP;
    }

    void *BasicMotor::getCapability(DeviceCapabilityId id)
    {
        if (id == CAP) return static_cast<BasicMotorControl *>(this);
        return nullptr;
    }

    bool BasicMotorFactory::matches(const Port &port) const
    {
        switch (port.getDeviceType())
        {
        case DeviceType::SIMPLE_MEDIUM_LINEAR_MOTOR:
        case DeviceType::TRAIN_MOTOR:
            return true;
        default:
            return false;
        }
    }
}
```

## 3. Multiple capabilities

A device can implement more than one capability interface:

```cpp
class EncoderMotor : public PortDevice, public EncoderMotorControl, public BasicMotorControl
{
    void *getCapability(DeviceCapabilityId id) override
    {
        if (id == EncoderMotor::CAP)  return static_cast<EncoderMotorControl *>(this);
        if (id == BasicMotor::CAP)    return static_cast<BasicMotorControl *>(this);
        return nullptr;
    }
};
```

## 4. Register

```cpp
// Once at startup, before using DeviceManager
Lpf2::Devices::BasicMotor::registerFactory(Lpf2::DeviceRegistry::instance());
```

Or include it in `DeviceRegistry::registerDefault()` if adding it to the library.
