#include "TrxStateVfo.h"

#include "I2Cinput.h"
#include "GPIOinput.h"

using enum mode;

TrxStateVfo::TrxStateVfo(I2Cinput& i2cInput, uint32_t rxFrequency)
:TrxState(i2cInput)
{
   setRxFrequency(rxFrequency);
}

void TrxStateVfo::setRxFrequency(uint32_t rxFrequency)
{
    if (rxFrequency < F_MIN)
    {
        this->rxFrequency = F_MIN;
    }
    else if (rxFrequency > F_MAX)
    {
        this->rxFrequency = F_MAX;
    }
    else
    {
        this->rxFrequency = rxFrequency;
    } 
}

uint32_t TrxStateVfo::getRxFrequency() const
{
    if (!(getI2Cinput().isPressedReverse()))
    {
        return rxFrequency;
    }
    else
    {
        auto offset = getI2Cinput().getDuplexOffset();
        return rxFrequency + offset;
    }
}

uint32_t TrxStateVfo::getTxFrequency() const
{
    if (!getI2Cinput().isPressedReverse())
    {
        auto offset = getI2Cinput().getDuplexOffset();
        return getRxFrequency() + offset;
    }
    else
    {
        return rxFrequency;
    }
}


bool TrxStateVfo::isCtcssOn() const
{
    return getI2Cinput().getMode() == ctcss;
}



void TrxStateVfo::up(int n)
{
    auto mode = getI2Cinput().getMode();

    if (mode != ctcss)
    {
        frequencyUp(n);
    }
    else // ctcss
    {
        ctcssUp(n);
    }
}


memory TrxStateVfo::toMemory() const
{
    struct memory m;
    m.rxFrequency = this->getRxFrequency();
    m.txFrequency = this->getTxFrequency();
    m.ctcssIndex = ctcssIndex;
    m.isCtcssOn = this->isCtcssOn();
    m.isUsed = true;
    return m;
}


void TrxStateVfo::ctcssUp(int n)
{
    if (n>=0)
    {
        ctcssIndex = (ctcssIndex + n) % ctcssValues.size();
    }
    else // n<0
    {
        if (ctcssIndex >= -n)
        {
            ctcssIndex += n; 
        }
        else
        {
            ctcssIndex = ctcssValues.size() + n + ctcssIndex;
        }
    }
}


void TrxStateVfo::frequencyUp(int n)
{
    if (getStep()<1000000)
    {
        rxFrequency -= (rxFrequency % getStep()); // step could have been changed
    }
    rxFrequency += (n * getStep());

    if (rxFrequency > F_MAX)
    {
        rxFrequency = F_MIN + rxFrequency - F_MAX - getStep();
    }
    else if (rxFrequency < F_MIN)
    {
        rxFrequency = F_MAX - (rxFrequency - F_MIN) - getStep();
    }
}
