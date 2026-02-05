#include "TrxStateSpecialMemoryFIR.h"

#include "Fir.h"

#include <sstream>
#include <iomanip>

TrxStateSpecialMemoryFIR::TrxStateSpecialMemoryFIR(I2Cinput& i2cInput, const TrxState *oldState)
    :TrxStateSpecialMemory(i2cInput), oldState(oldState)
{
    config.fHigh = Fir::getFMax();
    config.fNotch = 1000;     // 0 means off
    config.medianSize = 0; // 0 means off
}

void TrxStateSpecialMemoryFIR::update(const TrxState *oldState)
{
    this->oldState = oldState;
}

void TrxStateSpecialMemoryFIR::up(int n)
{
    if (n == 0)
    {
        return;
    }
    else
    {
        changed = true;

        if (focus == &(config.medianSize))
        {
            const int newSize = (int)config.medianSize + n;
            config.medianSize = std::max(1, newSize);
        }
        else if (focus == &(config.fHigh))
        {
            int newHigh = (int)config.fHigh + 10 * n;
            newHigh = std::min((int) Fir::getFMax(), newHigh);
            config.fHigh = std::max((int) Fir::getFMin(), newHigh);
        }
        else if (focus == &(config.fNotch))
        {
            const int newFNotch = config.fNotch + 10 * n;
            config.fNotch = std::max((int)Fir::getFMin(), newFNotch);
            config.fNotch = std::min(Fir::getFMax(), config.fNotch);
        }
    }
}

void TrxStateSpecialMemoryFIR::stepUp()
{
    changed = true;

    if (focus == &(config.fHigh))
    {
        focus = &(config.fNotch);
    }
    else if (focus == &(config.fNotch))
    {
        focus = &(config.medianSize);
    }
    else if (focus == &(config.medianSize))
    {
        focus = &(config.fHigh);
    }
}

void TrxStateSpecialMemoryFIR::stepDown()
{
    changed = true;

    if (focus == &(config.fHigh))
    {
        focus = &(config.fNotch);
    }
    else if (focus == &(config.fNotch))
    {
        focus = &(config.fHigh);
    }
    else if (focus == &(config.medianSize))
    {
        focus = &(config.fNotch);
    }
}

bool TrxStateSpecialMemoryFIR::isTxAllowed() const
{
    return oldState->isTxAllowed();
}

bool TrxStateSpecialMemoryFIR::isCtcssOn() const
{
    return oldState->isCtcssOn();
}

uint32_t TrxStateSpecialMemoryFIR::getTxFrequency() const
{
    return oldState->getTxFrequency();
}

uint32_t TrxStateSpecialMemoryFIR::getRxFrequency() const
{
    return oldState->getRxFrequency();
}

std::array<std::string, 3> TrxStateSpecialMemoryFIR::getInfo() const
{
    std::array<std::string, 3> lines;

    std::stringstream lowpassInfo;
    if (focus == &(config.fHigh))
    {
        lowpassInfo << 'L';
    }
    else
    {
        lowpassInfo << 'l';
    }
    lowpassInfo << std::right << std::setw(5) << config.fHigh;
    lines.at(0) = lowpassInfo.str();

    std::stringstream notchInfo;
    if (focus == &(config.fNotch))
    {
        notchInfo << 'N';
    }
    else
    {
        notchInfo << 'n';
    }
    notchInfo << std::right << std::setw(5) << config.fNotch;
    lines.at(1) = notchInfo.str();

    std::stringstream medianInfo;
    if (focus == &(config.medianSize))
    {
        medianInfo << 'M';
    }
    else
    {
        medianInfo << 'm';
    }
    medianInfo << std::right << std::setw(5) << config.medianSize;
    lines.at(2) = medianInfo.str();

    return lines;
}

filterConfig TrxStateSpecialMemoryFIR::getConfig() const
{
    return config;
}

bool TrxStateSpecialMemoryFIR::wasChanged()
{
    if (changed)
    {
        changed = false;
        return true;
    }
    else
    {
        return false;
    }
}