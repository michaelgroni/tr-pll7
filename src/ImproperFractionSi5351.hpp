#pragma once

#include "pico/stdlib.h"

template <typename T>
T gcd(T x, T y)
{
    if (x == 0)
        return y;
    if (y == 0)
        return x;
    if (x < y)
        return gcd(y, x);
    return gcd(y, x % y);
}

/*
 * a + b/c
 */
class ImproperFractionSi5351
{
private:    
    uint32_t a;
    uint32_t b;
    uint32_t c;

public:
    ImproperFractionSi5351(const double r);

    uint32_t getA() const;
    uint32_t getB() const;
    uint32_t getC() const;

    double getEpsilon() const;
};