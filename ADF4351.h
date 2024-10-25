#pragma once

#include "pico/stdlib.h"

class ADF4351
{
public:
    static ADF4351* getInstance();
    void write(const uint32_t frequency);

private:
    uint32_t oldPllFrequency = 0;

    ADF4351();                              
    ADF4351(const ADF4351&);                   // disable copy constructor              
    ADF4351 & operator = (const ADF4351 &);    // disable operator =

    uint32_t pllFrequency(uint32_t frequency);
    void writePLL(const uint8_t* values);
};

