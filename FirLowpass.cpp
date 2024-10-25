#include "FirLowpass.h"

#include <algorithm> // min(...)

using namespace std;

FirLowpass::FirLowpass(uint16_t fHigh, uint16_t fSample)
{
    set(fHigh, fSample);
}

void FirLowpass::set(uint16_t fHigh, uint16_t fSample)
{
    this->fLow = fLow;
    this->fSample = fSample;
    start = 0;

    for (auto &value : values)
    {
        value = 128;
    }

    // compute low-pass and high-pass
    int64_t lowpassSum = 0;

    for (size_t i = 0; i < FILTERSIZE; i++)
    {
        int_fast16_t cLow = sinc(2.0 * fHigh / fSample * (i - (FILTERSIZE - 1) / 2)) * INT16_MAX;
        coeffs.at(i) = cLow;
        lowpassSum += cLow;
    }

    // normalize
    for (size_t i = 0; i < FILTERSIZE; i++)
    {
        coeffs.at(i) = (int_fast16_t) (coeffs.at(i) * (double)INT16_MAX / lowpassSum);

    }
}

void FirLowpass::filter(uint16_t &sample)
{
    values[start] = sample;

    int_fast64_t result = 0;
    size_t i_values = start;

    for (size_t i = 0; i < FILTERSIZE; i++)
    {
        result += (int_fast64_t)values[i_values] * coeffs.at(i);

        if (i_values == 0)
        {
            i_values = FILTERSIZE - 1;
        }
        else
        {
            i_values--;
        }
    }
    sample = result >> 15;

    start = (start + 1) % FILTERSIZE;
}
