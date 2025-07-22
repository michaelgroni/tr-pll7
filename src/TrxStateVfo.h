#pragma once

#include "pico/stdlib.h"

#include "TrxState.h"
#include "memory.h"

class TrxStateVfo : public TrxState
{
public:
    TrxStateVfo(uint32_t rxFrequency);
    uint32_t getRxFrequency() const;
    void setRxFrequency(uint32_t frequency);
    uint32_t getTxFrequency() const;
    bool isCtcssOn() const;
    void up(int n);
    memory toMemory() const;

private:
    uint32_t rxFrequency;

    void frequencyUp(int n); // negative n for down
    void ctcssUp(int n); // negative n for down
};