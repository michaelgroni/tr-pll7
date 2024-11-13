#pragma once

#include "pico/stdlib.h"

class Si5351
{
public:
    static Si5351* getInstance();
    void write(const uint32_t frequency);

private:
    uint32_t oldPllFrequency = 0;

    Si5351();                              
    Si5351(const Si5351&);                   // disable copy constructor              
    Si5351 & operator = (const Si5351 &);    // disable operator =

    uint32_t pllFrequency(uint32_t frequency);
    void writePLL(const uint8_t* values);
};

