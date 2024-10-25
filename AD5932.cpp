#include "AD5932.h"

#include "settings.h"
#include "I2Cinput.h"
#include "I2Coutput.h"
#include "GPIOinput.h"

#include "hardware/spi.h"

AD5932* AD5932::getInstance()
{
    static AD5932 instance;
    return &instance;
}

void AD5932::write(const uint32_t frequency)
{
   const auto fDDS = ddsFrequency(frequency);

   if (fDDS != oldDdsFrequency)
   {
        I2Coutput::getInstance()->set_DDS_CTRL(false);
        sleep_ms(5);

        oldDdsFrequency = fDDS;

       // control register
        uint16_t data = 0x0FFF;
        writeDDS(data);

        // start frequency
        uint64_t fracDDS = (fDDS * (1L << 24)) / DDS_MCLK;
        //  12 LSBs
        data = 0xC000 | (fracDDS & 0x0FFF);
        writeDDS(data);
        //  12 MSBs
        data = 0xD000 | ((fracDDS >> 12) & 0x0FFF);
        writeDDS(data);

        // increment frequency := 2
        //  12 LSBs
        data = 0x2002;
        writeDDS(data);
        //  12 MSBs
        data = 0x3000;
        writeDDS(data);  

        // number of increments := 2 (minimum)
        data = 0x1002;
        writeDDS(data); 

        // increments interval := 2 MCLK periods (minimum)
        // data = 0x6002;
        // writeDDS(data); 

        I2Coutput::getInstance()->set_DDS_CTRL(true);
        I2Coutput::getInstance()->set_DDS_CTRL(false);
   }
}

AD5932::AD5932()
{
    // chip select
    gpio_init(DDS_OUT_LE);
    gpio_set_dir(DDS_OUT_LE, true); 
    gpio_put(DDS_OUT_LE, 1);
    
    // SPI only TX
    gpio_set_function(DDS_SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(DDS_SPI_TX,  GPIO_FUNC_SPI);
    spi_init(SPI_PORT, 100000);
    spi_set_format(SPI_PORT, 16, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

    write(435000000);
}

uint32_t AD5932::ddsFrequency(uint32_t frequency)
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

    return (frequency + 21600000) / 32;
}

void AD5932::writeDDS(const uint16_t &data)
{
    gpio_put(DDS_OUT_LE, 0);
    spi_write16_blocking(SPI_PORT, &data, 1);
    gpio_put(DDS_OUT_LE, 1);
}