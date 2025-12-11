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
    uint8_t getSpecialMemoryChannel();
    mode getMode();
    int32_t getDuplexOffset();
    bool isPllLocked();
    bool isPressedPtt();
    bool isPressedMR();
    bool isPressedMS();
    bool isPressedAB();
    bool wasPressedStepIncrease(); // push button
    bool wasPressedStepDecrease(); // push button
    bool wasPressedM();
    bool isPressedReverse();

private:
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;

    i2c_inst* i2cPort;

    mode getModePrivate();                              
};