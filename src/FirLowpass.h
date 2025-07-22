#pragma once

#include "pico/stdlib.h"

#include "Fir.h"

inline const size_t FILTERSIZE = 101;

// windowed-sinc filter
class FirLowpass : public Fir
{
private:
    uint_fast16_t fLow;
    std::array<int_fast16_t, FILTERSIZE> coeffs;
    
    uint8_t start = 0;
    std::array<uint_fast16_t, FILTERSIZE> values; // ring buffer   

public:
    FirLowpass(uint16_t fHigh, uint16_t fSample);
    void set(uint16_t fHigh, uint16_t fSample);
    void filter(uint16_t &sample);
};