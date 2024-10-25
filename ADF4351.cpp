#include "ADF4351.h"

#include "settings.h"
#include "I2Cinput.h"
#include "GPIOinput.h"

#include "hardware/spi.h"

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

        double nPll = fPll / 20000.0;
        uint32_t intPll = (int) nPll;
        uint fracPll = (int) (1000 * (nPll - intPll));
        uint32_t r0value = (intPll << 15) + (fracPll << 3);

        // write R5
        uint8_t r5[] = {0x00, 0x58, 0x00, 0x05};
        writePLL(r5);

        // write R4
        uint8_t r4[] = {0x00, 0x3F, 0xF0, 0x3C};
        writePLL(r4);
        
  
        // write R3
        uint8_t r3[] = {0x00, 0x00, 0x04, 0xB3};
        writePLL(r3);
    
        // write R2
        uint8_t r2[] = {0x19, 0x3E, 0xAE, 0x42};
        writePLL(r2);        
  
        // write R1
        uint8_t r1[] = {0x00, 0x00, 0x81, 0x91};
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

uint32_t ADF4351::pllFrequency(uint32_t frequency)
{
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
