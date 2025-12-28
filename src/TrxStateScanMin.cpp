#include "TrxStateScanMin.h"

#include "settings.h"
#include "memory.h"

TrxStateScanMin::TrxStateScanMin(I2Cinput& i2cInput)
:TrxStateSpecialMemory(i2cInput)
{}

uint32_t TrxStateScanMin::getRxFrequency() const
{
    return scanMin() + offset;
}

void TrxStateScanMin::up(int n)
{
    if (getStep() < 1000000) while ( getRxFrequency() % getStep() != 0) // step could have been changed
    {
        offset--;
    }

    offset += n*getStep();

    if (getRxFrequency() > F_MAX)
    {
        offset = F_MAX - scanMin();
    }
    else if (getRxFrequency() < F_MIN)
    {
        offset = F_MIN - scanMin();
    }
}

void TrxStateScanMin::save()
{
    saveScanMin(getRxFrequency());
    offset = 0;
}

bool TrxStateScanMin::isNotSaved() const
{
    return offset != 0;
}