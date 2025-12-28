#pragma once

#include "pico/stdlib.h"

#include <array>
#include <string>

#include "I2Cinput.h"

class TrxState
{
public:
    TrxState(I2Cinput& i2cInput);
    virtual uint32_t getRxFrequency() const = 0;
    virtual uint32_t getTxFrequency() const = 0;
    virtual void up(int n) = 0;
    virtual void stepUp();
    virtual void stepDown();
    unsigned int getStep() const;
    std::string getStepToString() const;

    virtual uint32_t getCurrentFrequency();
    virtual bool isTxAllowed() const;

    virtual bool isCtcssOn() const = 0;
    virtual double getCtcssFrequency() const;

    I2Cinput& getI2Cinput() const { return i2cInput; }

protected:
    // ctcss frequencies
    inline static constexpr std::array<double, 38> ctcssValues = {67, 71.9, 74.4, 77, 79.7, 82.5, 85.4, 88.5, 91.5, 94.8, 97.4, 100, 103.5,
        107.2, 110.9, 114.8, 118.8, 123, 127.3, 131.8, 136.5, 141.3, 146.2, 151.4, 156.7, 162.2, 167.9, 173.8, 179.9,
        186.2, 192.8, 203.5, 210.7, 218.1, 225.7, 233.6, 241.8, 250.3};
    size_t ctcssIndex = 20;

private:
    I2Cinput& i2cInput;

    // available steps
    const std::array<unsigned int, 8> steps = {0, 10, 100, 1000, 10000, 12500, 25000, 1000000}; // 0 means auto
    size_t stepIndex = 0; // 0 means auto
};