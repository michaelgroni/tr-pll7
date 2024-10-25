#pragma once

#include "TrxState.h"

class TrxStateSpecialMemory : public TrxState
{
public:
    virtual uint32_t getTxFrequency() const;
    virtual bool isTxAllowed() const;
    virtual bool isCtcssOn() const;
};