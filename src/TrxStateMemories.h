#pragma once

#include "pico/stdlib.h"

#include "TrxState.h"

class TrxStateMemories : public TrxState
{
public:
    TrxStateMemories();
    uint32_t getRxFrequency() const;
    uint32_t getTxFrequency() const;
    void up(int n);
    size_t getMemoryIndex() const;
    void stepUp();
    void stepDown();
    bool isCtcssOn() const;
    double getCtcssFrequency() const;
    void setWriteModeOn(bool writeModeOn);
    bool isWriteModeOn() const;
    bool isTxAllowed() const;
private:
    size_t memoryIndex; // 1 .. MEMORIES
    bool writeModeOn = false;
};

