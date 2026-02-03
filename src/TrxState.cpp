#include "TrxState.h"

#include "settings.h"
#include "GPIOinput.h"

using enum mode;

TrxState::TrxState(I2Cinput& i2cInput)
:i2cInput(i2cInput)
{}

uint32_t TrxState::getCurrentFrequency()
{
    if (i2cInput.isPressedPtt())
    {
        return getTxFrequency();
    }
    return getRxFrequency();
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
        auto mode = i2cInput.getMode();

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