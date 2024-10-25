#include "I2Coutput.h"


I2Coutput::I2Coutput()
{}

I2Coutput* I2Coutput::getInstance()
{
    static I2Coutput instance;
    return &instance;
}

void I2Coutput::set_DDS_CTRL(bool high)
{
    uint8_t ports;
    i2c_read_blocking(I2C_PORT, CONTROL_IC1_ADDR, &ports, 1, false);

    if (high)
    {
        ports = ports | 0x04;
    }
    else
    {
        ports = ports & 0xFB;
    }

    i2c_write_blocking(I2C_PORT, CONTROL_IC1_ADDR, &ports, 1, false);
}

void I2Coutput::pulse_DDS_CTRL()
{
    uint8_t ports;
    i2c_read_blocking(I2C_PORT, CONTROL_IC1_ADDR, &ports, 1, false);

    // high
    decltype(ports) out = ports | 0x04;
    i2c_write_blocking(I2C_PORT, CONTROL_IC1_ADDR, &out, 1, false);

    // low
    out = ports & 0xFB;
    i2c_write_blocking(I2C_PORT, CONTROL_IC1_ADDR, &out, 1, false);
}