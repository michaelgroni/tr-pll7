#include "TrxStateMemories.h"

#include "settings.h"
#include "memory.h"
#include "GPIOinput.h"
#include "GPIOoutput.h"

TrxStateMemories::TrxStateMemories(I2Cinput& i2cInput)
:TrxState(i2cInput)
{
    // find a used memory channel
    memoryIndex = MEMORIES;
    for (size_t i = 1; (i<=MEMORIES) && (memoryIndex == MEMORIES); i++)
    {
        if (flashData[i].isUsed)
        {
            memoryIndex = i;
        }
    }
}

uint32_t TrxStateMemories::getRxFrequency() const
{
    return flashData[memoryIndex].rxFrequency;
    // if  (flashData[memoryIndex].isUsed)
    // {
    //     if (!isPressed("reverse"))
    //     {
    //         return flashData[memoryIndex].rxFrequency;
    //     }
    //     else
    //     {
    //         return flashData[memoryIndex].txFrequency;
    //     }
    // }
    // else
    // {
    //     return 0;
    // }
}

uint32_t TrxStateMemories::getTxFrequency() const
{
    return flashData[memoryIndex].txFrequency;
    // if  (flashData[memoryIndex].isUsed)
    // {
    //     if (!isPressed("reverse"))
    //     {
    //         return flashData[memoryIndex].txFrequency;
    //     }
    //     else
    //     {
    //         return flashData[memoryIndex].rxFrequency;
    //     }
    // }
    // else
    // {
    //     return 0;
    // }
}

void TrxStateMemories::up(int n)
{
    if (n==0) return;

    const int step = n>0 ? 1 : -1;

    do
    {
        if ((memoryIndex == MEMORIES) && (step == 1))
        {
            memoryIndex = 1;
        }
        else if ((memoryIndex == 1) && (step == -1))
        {
            memoryIndex = MEMORIES;
        }
        else
        {
            memoryIndex += step;
        }

        if (flashData[memoryIndex].isUsed || isWriteModeOn())
        {
            n -= step;
        }
    } while (n != 0);
}

size_t TrxStateMemories::getMemoryIndex() const
{
    return memoryIndex;
}

void TrxStateMemories::stepUp()
{
    Piezo::getInstance()->beepError();
}

void TrxStateMemories::stepDown()
{
    Piezo::getInstance()->beepError();
}

bool TrxStateMemories::isCtcssOn() const
{
    return flashData[memoryIndex].isCtcssOn;
}

double TrxStateMemories::getCtcssFrequency() const
{
    return ctcssValues.at(flashData[memoryIndex].ctcssIndex);
}

bool TrxStateMemories::isWriteModeOn() const
{
    return writeModeOn;
}

void TrxStateMemories::setWriteModeOn(bool writeModeOn)
{
    this->writeModeOn = writeModeOn;
}

bool TrxStateMemories::isTxAllowed() const
{
    return !isWriteModeOn(); // TODO fixme
    // return (!isWriteModeOn()) && TrxState::isTxAllowed();
}