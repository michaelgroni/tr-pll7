#include "ImproperFractionSi5351.hpp"

#include <cmath>

ImproperFractionSi5351::ImproperFractionSi5351(double r)
{
    a = std::floor(r);

    double fraction = r - a;

    // Convert the fraction to a fraction with a denominator of less than 2^20
    c = (1 << 20) - 1;
    b = static_cast<uint32_t>(std::round(fraction * c));
    uint32_t g = gcd(b, c);
    b /= g;
    c /= g;
}

uint32_t ImproperFractionSi5351::getA() const
{
    return a;
}

uint32_t ImproperFractionSi5351::getB() const
{
    return b;
}

uint32_t ImproperFractionSi5351::getC() const
{
    return c;
}