#pragma once

#include "pico/stdlib.h"

#include "rp2040-Si5351/Si5351.hpp"

#include "I2Cinput.h"

#include <memory>

class ADF4351
{
public:
    ADF4351(I2Cinput& i2cInput, i2c_inst_t* i2cSi5351, const uint8_t i2cAddrSi5351);
    void write(const uint32_t frequency);

private:
    uint32_t frequency {0};
    bool isPttPressed {false};

    I2Cinput& i2cInput;

    i2c_inst_t* i2cSi5351;
    Si5351 si5351;
    void setupSi5351();

    uint32_t pllFrequency(uint32_t frequency) const;
    void writePLL(const uint8_t* values);
};
