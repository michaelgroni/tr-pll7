#pragma once

#include "settings.h"

enum mode {usb, lsb, fm2, ctcss};
// For the control unit CW ist the same as USB.
// The FM1 postion of the switch is used for CTCSS.

class I2Cinput
{
public:
    static I2Cinput* getInstance();
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

    mode getModePrivate();

    I2Cinput();                              
    I2Cinput(const I2Cinput&);                   // disable copy constructor              
    I2Cinput & operator = (const I2Cinput &);    // disable operator =
};