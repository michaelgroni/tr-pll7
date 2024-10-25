#include "Median.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <numeric>

using namespace std;

Median::Median(size_t filterSize)
    : filterSize(filterSize)
{
    for (size_t i = 0; i < filterSize; i++)
    {
        values.push_back(0);
    }
}

void Median::filter(uint_fast8_t &sample)
{
    values.erase(values.begin());
    values.push_back(sample);

    if (filterSize > 1)
    {
        vector valuesTemp(values);
        auto median = valuesTemp.begin() + filterSize / 2;
        nth_element(valuesTemp.begin(), median, valuesTemp.end());

        sample = *median;
    }
}

void Median::setSize(size_t filterSize)
{
    values.resize(filterSize, 128);
}