#pragma once

#include "pico/stdlib.h"


/*
 * a + b/c
 */
class ImproperFractionSi5351
{
private:    
    uint32_t a;
    uint32_t b;
    uint32_t c;

    static constexpr uint32_t gcd(uint32_t x, uint32_t y);

public:

    constexpr ImproperFractionSi5351(uint32_t a_, uint32_t b_, uint32_t c_);
    constexpr ImproperFractionSi5351(uint32_t b_, uint32_t c_);

    static constexpr ImproperFractionSi5351 add(uint32_t n, const ImproperFractionSi5351& f);
    static constexpr ImproperFractionSi5351 divide(uint32_t n, const ImproperFractionSi5351& ipf);

    uint32_t getA() const { return a; }
    uint32_t getB() const { return b; }
    uint32_t getC() const { return c; }
};

