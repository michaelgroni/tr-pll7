#pragma once

#include "TrxState.h"

class TrxStateSpecialMemory : public TrxState
{
public:
    TrxStateSpecialMemory(I2Cinput& i2cInput);
    virtual uint32_t getTxFrequency() const;
    virtual bool isTxAllowed() const;
    virtual bool isCtcssOn() const;
};