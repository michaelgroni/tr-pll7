#include "FirNotch.h"

using namespace std;

FirNotch::FirNotch(uint16_t fNotch, uint16_t fSample, uint16_t bandwidth)
{
    set(fNotch, fSample, bandwidth);
}

void FirNotch::set(uint16_t fNotch, uint16_t fSample, uint16_t bandwidth)
{
    this->fNotch = fNotch;
    this->fSample = fSample;
    uint_fast16_t fLow = fNotch - bandwidth / 2;
    uint_fast16_t fHigh = fNotch + bandwidth / 2;
    start = 0;

    for (auto &value : values)
    {
        value = 128;
    }

    // compute low-pass and high-pass
    array<int_fast16_t, 256> lowpass;
    int64_t lowpassSum = 0;

    array<int_fast16_t, 256> highpass;
    int64_t highpassSum = 0;

    for (size_t i = 0; i < 256; i++)
    {
        int_fast16_t cLow = sinc(2.0 * fLow / fSample * (i - 128)) * INT16_MAX;
        lowpass.at(i) = cLow;
        lowpassSum += cLow;

        int_fast16_t cHigh = sinc(2.0 * fHigh / fSample * (i - 128)) * INT16_MAX;
        highpass.at(i) = cHigh;
        highpassSum += cHigh;
    }

    // normalize
    for (size_t i = 0; i < 256; i++)
    {
        lowpass.at(i) = (int_fast16_t)(lowpass.at(i) * (double)INT16_MAX / lowpassSum);
        highpass.at(i) = (int_fast16_t)(highpass.at(i) * (double)INT16_MAX / -highpassSum);
    }

    // peak
    highpass.at(128) += INT16_MAX;

    // add  both filters
    for (size_t i = 0; i < 256; i++)
    {
        coeffs.at(i) = highpass.at(i) + lowpass.at(i);
    }
}

uint16_t FirNotch::getFNotch() const
{
    return fNotch;
}

void FirNotch::setFNotch(uint16_t fNotch)
{
    set(fNotch, fSample, bandwidth);
}

void FirNotch::filter(uint8_t &sample)
{
    values[start] = sample;
    start++;

    int_fast32_t result = 0;
    uint8_t i_values = start - 1;

    for (const auto coeff : coeffs)
    {
        result += (int_fast32_t)values[i_values--] * coeff;
    }

    sample = result >> 15;
}
