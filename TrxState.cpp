#include "TrxState.h"

#include "settings.h"
#include "GPIOinput.h"
#include "I2Cinput.h"


uint32_t TrxState::getCurrentFrequency() const
{
    if (I2Cinput::getInstance()->isPressedPtt())
    {
        return getTxFrequency();
    }
    else
    {
        return getRxFrequency();
    }
}

void TrxState::stepUp()
{
    if (stepIndex == steps.size() - 1)
    {
        stepIndex = 0;
    }
    else
    {
        stepIndex++;
    }
}


void TrxState::stepDown()
{
    if (stepIndex == 0)
    {
        stepIndex = steps.size() -1;
    }
    else
    {
        stepIndex--;
    }
}

unsigned int TrxState::getStep() const
{
    if (stepIndex==0) // auto
    {
        auto mode = I2Cinput::getInstance()->getMode();

        if ((mode==fm2) || (mode==ctcss))
        {
            return 12500;
        }
        else
        {
            return 1000;
        }
    }
    else
    {
        return steps.at(stepIndex);
    }
}


std::string TrxState::getStepToString() const
{
    if (stepIndex == 0)
    {
        return "auto";
    }
    else
    {
        return std::to_string(getStep());
    }
}


bool TrxState::isTxAllowed() const
{
    auto const txf = getTxFrequency();
    return (txf >= F_MIN_TX) && (txf <= F_MAX_TX);
}


double TrxState::getCtcssFrequency() const
{
    return ctcssValues.at(ctcssIndex);
}