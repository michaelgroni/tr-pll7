#include "ADF4351.h"

#include "settings.h"
#include "I2Cinput.h"
#include "GPIOinput.h"
#include "ImproperFractionSi5351.hpp"
#include "rp2040-Si5351/Si5351.hpp"

#include "hardware/spi.h"

// SI5351 and ADF4351
const uint_fast32_t F_XO_SI = 27000000;
const uint_fast32_t F_VCO_SI = 33 * F_XO_SI; // Must bei a multiple of F_XO_SI.
const uint_fast32_t PFD_ADF = 100000; // 100 kHz
const uint_fast8_t R_ADF = 100;

using namespace std;

ADF4351* ADF4351::getInstance()
{
    static ADF4351 instance;
    return &instance;
}

void ADF4351::write(const uint32_t frequency)
{
   auto fPll = pllFrequency(frequency);

   if (fPll != oldPllFrequency)
   {
        oldPllFrequency = fPll;

        const uint_fast16_t offsetADF = fPll % PFD_ADF;
        const uint_fast32_t freqADF = fPll - offsetADF;
        const uint_fast16_t nADF = freqADF / PFD_ADF;
        ImproperFractionSi5351 fRefADF(PFD_ADF * R_ADF, offsetADF * R_ADF, nADF);

        // Si5351 multisynth
        ImproperFractionSi5351 mMultisynth = ImproperFractionSi5351::divide(F_VCO_SI, fRefADF);
        const auto ma = mMultisynth.getA();     
        const auto mb = mMultisynth.getB();      // 20 bits
        const auto mc = mMultisynth.getC();      // 20 bits
        
        // TODO to be continued here

        
        uint32_t r0value = (intPll << 15) + (fracPll << 3);

        // write R5
        uint8_t r5[] = {0x00, 0x58, 0x00, 0x05};
        writePLL(r5);

        // write R4
        uint8_t r4[] = {0x00, 0x30, 0x10, 0x3C};
        writePLL(r4);
        
  
        // write R3
        uint8_t r3[] = {0x00, 0x00, 0x04, 0xB3};
        writePLL(r3);
    
        // write R2
        uint8_t r2[] = {0x19, 0x9C, 0x4E, 0x42};
        writePLL(r2);        
  
        // write R1
        uint8_t r1[] = {0x08, 0x00, 0x8C, 0xA1};
        writePLL(r1);
       
        // write R0
        uint8_t r0[4];
        uint8_t* fake8bit = (uint8_t*) &r0value;
        for (size_t i = 0; i<4; i++)
        {
            r0[3-i] = *(fake8bit+i);
        }
        writePLL(r0);
   }
}

ADF4351::ADF4351()
{
    // SPI only TX
    gpio_set_function(PLL_SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PLL_SPI_TX,  GPIO_FUNC_SPI);
    spi_init(SPI_PORT, 1000000);

    // chip select
    gpio_init(PLL_OUT_LE);
    gpio_set_dir(PLL_OUT_LE, true); 
    gpio_put(PLL_OUT_LE, 1);
}

uint32_t ADF4351::pllFrequency(uint32_t frequency) const
{
    // PLL frequency = 21600000 + frequency + 1500 (USB) or -1500 (LSB)
    // 21600000 is the offset for the ADF4351 PLL
    // 1500 is the offset for the I2Cinput mode (USB/LSB)

    if (frequency < 1000000 || frequency > 60000000)
        return oldPllFrequency; // invalid frequency

    if (frequency == oldPllFrequency)
        return oldPllFrequency; // no change

    auto mode = I2Cinput::getInstance()->getMode();

    switch (mode)
    {
        case usb: // or cw
           frequency += 1500;
           break;
        case lsb:
           frequency -= 1500;
           break;
    }

    return frequency + 21600000;
}

void ADF4351::writePLL(const uint8_t *values)
{
    gpio_put(PLL_OUT_LE, 0);
    spi_write_blocking(SPI_PORT, values, 4);
    gpio_put(PLL_OUT_LE, 1);
}

void ADF4351::computeDividers(const uint32_t &pllFrequency,
    uint_fast8_t &ma, uint_fast32_t &mb, uint_fast32_t &mc, uint_fast16_t nADF) const
{
    const int offsetADF = pllFrequency % PFD_ADF;
    const int freqADF = pllFrequency - offsetADF;
    nADF = freqADF / PFD_ADF;
    ImproperFractionSi5351 fRefADF(PFD_ADF * R_ADF, offsetADF * R_ADF, nADF);

    // Si5351 multisynth
    ImproperFractionSi5351 mMultisynth = ImproperFractionSi5351::divide(F_VCO_SI, fRefADF);
    ma = mMultisynth.getA();
    mb = mMultisynth.getB();
    mc = mMultisynth.getC();
}