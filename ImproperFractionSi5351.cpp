#include "ImproperFractionSi5351.hpp"

#include <cmath>

ImproperFractionSi5351::ImproperFractionSi5351(uint32_t a, uint32_t b, uint32_t c)
    : a(0), b(0), c(1)
{
    if (b >= c)
    {
        const auto n = b / c;
        a += n;
        b -= n * c;
    }

    const auto g = gcd(b, c);
    b = b / g;
    c = c / g;

    this->a = a;
    this->b = b;
    this->c = c;
}

ImproperFractionSi5351::ImproperFractionSi5351(uint32_t b_, uint32_t c_)
    : ImproperFractionSi5351(0, b_, c_) {}

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

ImproperFractionSi5351 operator/(const uint32_t n, const ImproperFractionSi5351& ipf)
{
    const auto ipfA = ipf.getA();
    const auto ipfB = ipf.getB();
    const auto ipfC = ipf.getC();

    double result = n / (ipfA + (double) ipfB / ipfC);

    uint32_t a = std::floor(result);

    double fraction = result - a;
    uint64_t c = 100'000'000;
    uint64_t b = fraction * 100'000'000;

    auto g = gcd(b, c);
    c = c / g;
    b = b / g;

    return ImproperFractionSi5351(a, b, c);
}