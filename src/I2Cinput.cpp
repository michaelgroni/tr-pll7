#include "I2Cinput.h"

#include "hardware/timer.h"

I2Cinput::I2Cinput(i2c_inst* i2Port)
:i2cPort(i2cPort)
{}

void I2Cinput::update() // must be called in the main loop
{
    
    i2c_read_timeout_us(i2cPort, ENCODER_IC1_ADDR, &byte1, 1, false, 20000);
    // i2c_read_blocking(i2cPort, ENCODER_IC1_ADDR, &byte1, 1, false);
    __asm volatile("bkpt 1");
    i2c_read_blocking(i2cPort, ENCODER_IC2_ADDR, &byte2, 1, false);
    i2c_read_blocking(i2cPort, CONTROL_IC1_ADDR, &byte3, 1, false);
}

uint8_t I2Cinput::getSpecialMemoryChannel() // memory switch
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
        return ctcss;
    if ((byte1 & 32) == 0)
        return fm2;
    return lsb;
}

int32_t I2Cinput::getDuplexOffset()
{
    mode m = getMode();

    if ((m==fm2) || (m==ctcss))
    {    
        if ((byte3 & 1) == 0)
            return -7600000;
        if ((byte3 & 2) == 0)
            return -9400000;
    }
    
    return 0;
}

bool I2Cinput::isPllLocked()
{
    return (byte3 & 4) == 1;
}

bool I2Cinput::isPressedPtt()
{
    return (byte3 & 8) == 0;
}

bool I2Cinput::isPressedMR()
{
    return (byte2 & 128) == 0;
}

bool I2Cinput::isPressedMS()
{
    return (byte2 & 64) == 0;
}

bool I2Cinput::isPressedAB()
{
    return (byte2 & 32) == 0;
}

bool I2Cinput::wasPressedStepIncrease() // push button
{
    static bool isPressed = false;

    if ((byte2 & 16) == 0) //pressed
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

bool I2Cinput::wasPressedStepDecrease()
{
    static bool isPressed = false;

    if ((byte2 & 8) == 0) //pressed
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

bool I2Cinput::wasPressedM()
{
    static bool isPressed = false;

    if ((byte2 & 2) == 0) //pressed
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


bool I2Cinput::isPressedReverse() // former MHz switch
{
    return (byte2 & 1) == 0;
}