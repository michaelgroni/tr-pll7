#pragma once

#include "TrxStateSpecialMemory.h"
#include <string>
#include <array>

struct filterConfig
{
    uint16_t fHigh;
    uint16_t fNotch;
    uint16_t medianSize;
};

class TrxStateSpecialMemoryFIR : public TrxStateSpecialMemory
{
private:
    const TrxState* oldState;
    filterConfig config;
    uint16_t* focus = &(config.fHigh);
    bool changed = true;
public:
    TrxStateSpecialMemoryFIR(const TrxState* oldState);
    void update(const TrxState* oldState);
    void up(int n);
    void stepUp();
    void stepDown();
    bool isTxAllowed() const;
    bool isCtcssOn() const;
    uint32_t getTxFrequency() const;
    uint32_t getRxFrequency() const;
    std::array<std::string, 3> getInfo() const;
    filterConfig getConfig() const;
    bool wasChanged();
};