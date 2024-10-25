#pragma once

#include "pico/stdlib.h"

#include <vector>
#include <string>

// median filter
class Median
{
private:
    std::vector<uint_fast8_t> values;
    size_t filterSize;

public:
    Median(size_t filterSize); 
    void setSize(size_t filterSize);
    void filter(uint_fast8_t &sample);
    static size_t getMaxSize() {return 20;};
};