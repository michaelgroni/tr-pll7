#include "ImproperFractionSi5351.hpp"

#include <algorithm>
#include <cmath>

ImproperFractionSi5351::ImproperFractionSi5351(const uint32_t fXO, const uint8_t rADF, const uint32_t pfdADF, const uint32_t fEnd)
{
    const uint32_t offsetADF = fEnd % pfdADF;
    const uint32_t fADF = fEnd - offsetADF;

    nADF = fADF / pfdADF;

    bool finished = false;
    float bestError = __FLT_MAX__;

    for (int32_t k = K_MAX; (k >= K_MIN) && !finished; k--)
    // for (int32_t k = K_MIN; (k <= K_MAX) && !finished; k++)
    {
        for (uint8_t mCandidate = 60; (mCandidate <= 90) && !finished; mCandidate += 2)
        {
            const uint32_t cCandidate = k * STEP;

            // a and b candidate
            const uint64_t num = (uint64_t)mCandidate * rADF * ((uint64_t)pfdADF * nADF + offsetADF);
            const uint64_t den = (uint64_t)fXO * nADF;
            const uint32_t aCandidate = num / den;
            const uint64_t rem = num % den;
            const uint32_t bCandidate0 = static_cast<uint32_t>((rem * cCandidate) / den);                       // floored
            const uint32_t bCandidate1 = (bCandidate0 < cCandidate) ? bCandidate0 + 1 : bCandidate0; // try next higher b, if possible

            const double fCandidate0 = (fXO * (aCandidate + (double)bCandidate0 / cCandidate)) * (nADF / ((double)rADF * mCandidate));
            const double fCandidate1 = (fXO * (aCandidate + (double)bCandidate1 / cCandidate)) * (nADF / ((double)rADF * mCandidate));
            const float error0 = std::abs(static_cast<float>(fEnd) - fCandidate0);
            const float error1 = std::abs(static_cast<float>(fEnd) - fCandidate1);

            const float errorCandidate = std::min(error0, error1);

            if (errorCandidate < bestError)
            {
                bestError = errorCandidate;
                a = aCandidate;
                b = (error0 < error1) ? bCandidate0 : bCandidate1;
                c = cCandidate;
                m = mCandidate;
                error = errorCandidate;

                if (error <= 0.1)
                {
                    finished = true;
                }
            }
        }
    }
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

uint8_t ImproperFractionSi5351::getM() const
{
    return m;
}

uint16_t ImproperFractionSi5351::getNADF() const
{
    return nADF;
}

double ImproperFractionSi5351::getEpsilon() const
{
    double frac = static_cast<double>(b) / static_cast<double>(c);
    return std::min(frac, 1.0 - frac);
}

float ImproperFractionSi5351::getError() const
{
    return error;
}

std::string ImproperFractionSi5351::toString()
{
    return std::to_string(a) + " + " + std::to_string(b) + "/" + std::to_string(c) + " (m=" + std::to_string(m) + "; error=" + std::to_string(error) + ")";
}