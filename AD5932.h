#pragma once

#include "pico/stdlib.h"

class AD5932
{
public:
    static AD5932* getInstance();
    void write(const uint32_t frequency);

private:
    uint32_t oldDdsFrequency = 0;

    AD5932();                              
    AD5932(const AD5932&);                   // disable copy constructor              
    AD5932 & operator = (const AD5932 &);    // disable operator =

    uint32_t ddsFrequency(uint32_t frequency);
    void writeDDS(const uint16_t &data);
};
