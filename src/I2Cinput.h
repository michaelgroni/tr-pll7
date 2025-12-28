#pragma once

#include "settings.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"

enum mode {usb, lsb, fm2, ctcss};
// For the control unit CW ist the same as USB.
// The FM1 postion of the switch is used for CTCSS.

class I2Cinput
{
public:
    I2Cinput(i2c_inst* i2cPort);
    void update(); // must be called in the main loop
    uint8_t getSpecialMemoryChannel() const;
    mode getMode();
    int32_t getDuplexOffset();
    bool isPllLocked() const;
    bool isPressedPtt() const;
    bool isPressedMR() const;
    bool isPressedMS() const;
    bool isPressedAB() const;
    bool wasPressedStepIncrease() const; // push button
    bool wasPressedStepDecrease() const; // push button
    bool wasPressedM() const;
    bool isPressedReverse() const;

private:
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;

    i2c_inst* i2cPort;

    mode getModePrivate();                              
};