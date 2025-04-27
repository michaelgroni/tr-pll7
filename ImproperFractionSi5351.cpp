#include "ImproperFractionSi5351.hpp"

uint32_t ImproperFractionSi5351::gcd(uint32_t x, uint32_t y)
{
    if (x==0)
        return y;
    if (y==0)
        return x;
    if (x<y)
        return gcd(y, x);
    return gcd(y, x%y);
}

ImproperFractionSi5351::ImproperFractionSi5351(uint32_t a, uint32_t b, uint32_t c)
: a(0), b(0), c(1)
{
    if (c == 0) return; // division by zero → undefined

    if (b >= c)
    {
        uint32_t n = b / c;
        a += n;
        b -= n * c;
    }

    if (b > 3 && c > 3)
    {
        uint32_t g = gcd(b, c);
        b /= g;
        c /= g;
    }

    this->a = a;
    this->b = b;
    this->c = c;
}

ImproperFractionSi5351::ImproperFractionSi5351(uint32_t b_, uint32_t c_)
    : ImproperFractionSi5351(0, b_, c_) {}

ImproperFractionSi5351 ImproperFractionSi5351::add(uint32_t n, const ImproperFractionSi5351& f)
{
    auto a = n + f.getA();
    auto b = f.getB();
    auto c = f.getC();
    return ImproperFractionSi5351(a, b, c);
}

ImproperFractionSi5351 ImproperFractionSi5351::divide(uint32_t n, const ImproperFractionSi5351& ipf)
{
    auto a = ipf.getA();
    auto b = ipf.getB();
    auto c = ipf.getC();

    uint64_t newB = static_cast<uint64_t>(n) * c;
    uint64_t newC = static_cast<uint64_t>(a) * c + b;

    uint32_t newA = 0;
    while (newB >= newC) {
        newA++;
        newB -= newC;
    }

    int bLen = 64 - __builtin_clzll(newB);
    int cLen = 64 - __builtin_clzll(newC);
    const int maxLen = 20;

    if (bLen > maxLen || cLen > maxLen) // Si5351 restriction
    {
        int shift = (bLen > cLen ? bLen : cLen) - maxLen;
        newB >>= shift;
        newC >>= shift;
    }

    return ImproperFractionSi5351(newA, static_cast<uint32_t>(newB), static_cast<uint32_t>(newC));
}
