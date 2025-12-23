#include "ImproperFractionSi5351.hpp"

#include <algorithm>
#include <cmath>

ImproperFractionSi5351::ImproperFractionSi5351(const double r)
    : a(std::floor(r))
{
    const double fraction = r - a;

    // Convert the fraction to a fraction with a denominator of less than 2^20
    c = (1 << 20) - 1;
    b = static_cast<uint32_t>(std::round(fraction * c));

    if (b == c)
    {
        a += 1;
        b = 0;
        c = 1;
    }
    // else
    // {
    //     const uint32_t g = gcd(b, c);
    //     b /= g;
    //     c /= g;
    // }
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

double ImproperFractionSi5351::getEpsilon() const
{
    double frac = static_cast<double>(b) / static_cast<double>(c);
    return std::min(frac, 1.0 - frac);
}

std::string ImproperFractionSi5351::toString()
{
    return std::to_string(a) + " + " + std::to_string(b) + "/" + std::to_string(c);
}   