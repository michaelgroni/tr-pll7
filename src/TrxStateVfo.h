#pragma once

#include "pico/stdlib.h"

#include "TrxState.h"
#include "memory.h"

class TrxStateVfo : public TrxState
{
public:
    TrxStateVfo(I2Cinput& i2cInput, uint32_t rxFrequency);
    uint32_t getRxFrequency() const;
    void setRxFrequency(uint32_t frequency);
    uint32_t getTxFrequency() const;
    bool isCtcssOn() const;
    void up(const int n);
    memory toMemory() const;

private:
    uint32_t rxFrequency;

    void ctcssUp(const int n); // negative n for down
    void frequencyUp(const int n); // negative n for down
};