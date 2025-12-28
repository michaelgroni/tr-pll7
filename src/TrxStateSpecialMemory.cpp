#include "TrxStateSpecialMemory.h"

TrxStateSpecialMemory::TrxStateSpecialMemory(I2Cinput& i2cInput)
:TrxState(i2cInput)
{}

uint32_t TrxStateSpecialMemory::getTxFrequency() const
{
    return getRxFrequency();
}

bool TrxStateSpecialMemory::isTxAllowed() const
{
    return false;
}

bool TrxStateSpecialMemory::isCtcssOn() const
{
    return false;
}
