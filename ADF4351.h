#pragma once

#include "pico/stdlib.h"

#include "rp2040-Si5351/Si5351.hpp"

#include <array>

class ADF4351
{
public:
    static ADF4351* getInstance();
    void write(const uint32_t frequency);

private:
    uint32_t oldPllFrequency {0};

    ADF4351();                              
    ADF4351(const ADF4351&);                   // disable copy constructor              
    ADF4351 & operator = (const ADF4351 &);    // disable operator =

    Si5351 si5351;
    void setupSi5351(Si5351 &si5351);

    uint32_t pllFrequency(uint32_t frequency) const;
    void writePLL(const uint8_t* values);
};

