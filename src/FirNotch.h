#pragma once

#include "pico/stdlib.h"

#include "Fir.h"

// windowed-sinc filter
class FirNotch : public Fir
{
private:
    uint_fast16_t fNotch;
    uint_fast16_t bandwidth;
    std::array<int_fast16_t, 256> coeffs;

    uint8_t start = 0;
    std::array<uint_fast8_t, 255> values; // ring buffer    

public:
    FirNotch(uint16_t fNotch, uint16_t fSample, uint16_t bandwidth = 80);
    void set(uint16_t fNotch, uint16_t fSample, uint16_t bandwidth = 80);
    uint16_t getFNotch() const;
    void setFNotch(uint16_t fNotch);
    void filter(uint8_t &sample);
};