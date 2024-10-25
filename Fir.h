#pragma once

#include "pico/stdlib.h"

#include <array>

// windowed-sinc filter
class Fir
{
protected:
    uint16_t fSample;
    double sinc(double x);
public:
    static uint16_t getFMin() {return 50;};
    static uint16_t getFMax() {return 5000;};
};
