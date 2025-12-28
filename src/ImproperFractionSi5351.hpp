#pragma once

#include "pico/stdlib.h"
#include <string>

/*
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
*/

/*
 * a + b/c
 */
class ImproperFractionSi5351
{
private:    
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint8_t m;
    uint16_t nADF;
    double error;

    static constexpr uint16_t STEP {1};
    static constexpr uint32_t MAX_DENOMINATOR { (1UL << 20) - 1};
    // static constexpr uint32_t MAX_DENOMINATOR { 10000 };
    static constexpr uint32_t K_MAX = MAX_DENOMINATOR / STEP;
    static constexpr uint32_t K_MIN = (K_MAX / 2) -1;
    
public:
    ImproperFractionSi5351(const uint32_t fXO, const uint8_t rADF, const uint32_t pfdADF, const uint32_t fEnd);

    uint32_t getA() const;
    uint32_t getB() const;
    uint32_t getC() const;
    uint8_t getM() const;
    uint16_t getNADF() const;

    double getEpsilon() const;
    double getError() const;

    std::string toString();
};