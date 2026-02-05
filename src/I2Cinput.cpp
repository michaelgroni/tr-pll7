#include "I2Cinput.h"

using enum mode;

I2Cinput::I2Cinput(i2c_inst *i2cPort)
    : i2cPort(i2cPort)
{}

void I2Cinput::update() // must be called in the main loop
{
    i2c_read_blocking(i2cPort, ENCODER_IC1_ADDR, &byte1, 1, false);
    i2c_read_blocking(i2cPort, ENCODER_IC2_ADDR, &byte2, 1, false);
    i2c_read_blocking(i2cPort, CONTROL_IC1_ADDR, &byte3, 1, false);
}

uint8_t I2Cinput::getSpecialMemoryChannel() const // memory switch
{
    if ((byte1 & 1) == 0)
        return 6;
    if ((byte1 & 2) == 0)
        return 5;
    if ((byte1 & 64) == 0)
        return 4;
    if ((byte1 & 4) == 0)
        return 3;
    if ((byte1 & 8) == 0)
        return 2;
    return 1;
}

mode I2Cinput::getMode()
{
    mode m = getModePrivate();

    if (m == lsb) // mode is LSB or the switch is being moved at the moment
    {
        sleep_ms(6);
        i2c_read_blocking(i2cPort, ENCODER_IC1_ADDR, &byte1, 1, false);
        return getModePrivate();
    }
    else
    {
        return m;
    }
}

mode I2Cinput::getModePrivate()
{
    if ((byte1 & 128) == 0)
        return usb;
    if ((byte1 & 16) == 0)
    {
        return ctcss;
    }
    if ((byte1 & 32) == 0)
        return fm2;
    return lsb;
}

int32_t I2Cinput::getDuplexOffset()
{
    mode m = getMode();

    if ((m == fm2) || (m == ctcss))
    {
        if ((byte3 & 1) == 0)
            return -7600000;
        if ((byte3 & 2) == 0)
            return -9400000;
    }

    return 0;
}

bool I2Cinput::isPllLocked() const
{
    return (byte3 & 4) == 1;
}

bool I2Cinput::isPressedPtt() const
{
    return (byte3 & 8) == 0;
}

bool I2Cinput::isPressedMR() const
{
    return (byte2 & 128) == 0;
}

bool I2Cinput::isPressedMS() const
{
    return (byte2 & 64) == 0;
}

bool I2Cinput::isPressedAB() const
{
    return (byte2 & 32) == 0;
}

bool I2Cinput::wasPressedStepIncrease() const // push button
{
    static bool isPressed = false;

    if ((byte2 & 16) == 0) // pressed
    {
        if (isPressed) // true was already returned
        {
            return false;
        }
        else
        {
            isPressed = true;
            return true;
        }
    }
    else // not pressed
    {
        isPressed = false;
        return false;
    }
}

bool I2Cinput::wasPressedStepDecrease() const
{
    static bool isPressed = false;

    if ((byte2 & 8) == 0) // pressed
    {
        if (isPressed) // true was already returned
        {
            return false;
        }
        else
        {
            isPressed = true;
            return true;
        }
    }
    else // not pressed
    {
        isPressed = false;
        return false;
    }
}

bool I2Cinput::wasPressedM() const
{
    static bool isPressed = false;

    if ((byte2 & 2) == 0) // pressed
    {
        if (isPressed) // true was already returned
        {
            return false;
        }
        else
        {
            isPressed = true;
            return true;
        }
    }
    else // not pressed
    {
        isPressed = false;
        return false;
    }
}

bool I2Cinput::isPressedReverse() const // former MHz switch
{
    return (byte2 & 1) == 0;
}