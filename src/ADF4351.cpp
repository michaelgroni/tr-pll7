#include "ADF4351.h"

#include "settings.h"
#include "I2Cinput.h"
#include "GPIOinput.h"
#include "ImproperFractionSi5351.hpp"
#include "rp2040-Si5351/Si5351.hpp"

#include "hardware/spi.h"

// SPI PLL
const auto SPI_PORT = spi0;
const uint8_t PLL_SPI_SCK = 6; // SCLK
const uint8_t PLL_SPI_TX = 7;  // MOSI
const uint8_t PLL_OUT_LE = 4;

// SI5351
const uint_fast32_t F_XO_SI = 25'000'000;
const uint_fast8_t M_MULTISYNTH {90};

// ADF4351
const uint_fast32_t PFD_ADF = 100'000; // 100 kHz
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

        const uint_fast32_t offsetADF = fPll % PFD_ADF;
        const uint_fast32_t freqADF = fPll - offsetADF;
        const uint_fast16_t nADF = freqADF / PFD_ADF;
        const double fRefADF = R_ADF * (PFD_ADF + (double) offsetADF/nADF);

        // Si5351 PLL_A      
        const double fPllSi = fRefADF * M_MULTISYNTH;
        ImproperFractionSi5351 nPLLSi(fPllSi / F_XO_SI);
        const auto na = nPLLSi.getA();     
        const auto nb = nPLLSi.getB();      // 20 bits
        const auto nc = nPLLSi.getC();      // 20 bits
        si5351.setPllParameters('a', na, nb, nc);
        // si5351.resetPll('a');

        sleep_ms(2); // wait for the Si5351 to be ready

        // write R5
        uint8_t r5[] = {0x00, 0x58, 0x00, 0x05};
        writePLL(r5);

        // write R4
        uint8_t r4[] = {0x00, 0x30, 0x24, 0x2C};
        writePLL(r4);
        
  
        // write R3
        uint8_t r3[] = {0x00, 0xE1, 0x00, 0x13};
        writePLL(r3);
    
        // write R2
        uint8_t r2[] = {0x18, 0x19, 0x1F, 0xC2};
        writePLL(r2);        
  
        // write R1
        uint8_t r1[] = {0x08, 0x00, 0x80, 0x11};
        writePLL(r1);
       
        // write R0
        uint32_t r0value = ((uint32_t) nADF) << 15;
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

    setupSi5351(si5351);
}

void ADF4351::setupSi5351(Si5351 &si5351)
{
    si5351.setClkControl(0, false, true, 0, false, 3, 8);
    si5351.setPllInputSource(1);
    si5351.setPllParameters('a', 900'000'000/F_XO_SI, 0, 15);
    si5351.resetPll();
    si5351.setMultisynth0to5parameters(0, M_MULTISYNTH, 0, 15);
    si5351.setOutput(0, true);

    // power down unused outputs (see AN619 chapter 4.2.5)
    for (uint8_t i = 1; i <= 7; i++)
    {
        si5351.setClkControl(i, true, true, 0, false, 3, 2);
    }
}

uint32_t ADF4351::pllFrequency(uint32_t frequency) const
{
    // PLL frequency = 21600000 + frequency +1500 (USB) or -1500 (LSB)

    const auto mode = I2Cinput::getInstance()->getMode();

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