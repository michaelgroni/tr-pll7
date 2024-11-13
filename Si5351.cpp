#include "Si5351.h"

#include "settings.h"
#include "I2Cinput.h"
#include "GPIOinput.h"

#include "hardware/i2c.h"

#include <array>

const uint8_t I2C1_ADDR = 0x30;

Si5351 *Si5351::getInstance()
{
    static Si5351 instance;
    return &instance;
}

void Si5351::write(const uint32_t frequency)
{
    auto fPll = pllFrequency(frequency);

    if (fPll != oldPllFrequency)
    {
        oldPllFrequency = fPll;

        double nPll = fPll / 20000.0;
        uint32_t intPll = (int)nPll;
        uint fracPll = (int)(2000 * (nPll - intPll));
        uint32_t r0value = (intPll << 15) + (fracPll << 3);

        // write R5
        uint8_t r5[] = {0x00, 0x58, 0x00, 0x05};
        writePLL(r5);

        // write R4
        uint8_t r4[] = {0x00, 0x30, 0x12, 0x3C};
        writePLL(r4);

        // write R3
        uint8_t r3[] = {0x00, 0x04, 0x04, 0xB3};
        writePLL(r3);

        // write R2
        uint8_t r2[] = {0x19, 0x3E, 0xBE, 0x42};
        writePLL(r2);

        // write R1
        uint8_t r1[] = {0x08, 0x00, 0xBE, 0x81};
        writePLL(r1);

        // write R0
        uint8_t r0[4];
        uint8_t *fake8bit = (uint8_t *)&r0value;
        for (size_t i = 0; i < 4; i++)
        {
            r0[3 - i] = *(fake8bit + i);
        }
        writePLL(r0);
    }
}

Si5351::Si5351()
{
    i2c_init(i2c1, I2C_CLOCK);
    gpio_set_function(PLL_I2C1_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PLL_I2C1_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PLL_I2C1_SDA);
    gpio_pull_up(PLL_I2C1_SCL);

    std::array<uint8_t, 233> registerMap = {
        0x00,
        0x00,
        0x18,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x4F,
        0x80,
        0x80,
        0x80,
        0x80,
        0x80,
        0x80,
        0x80,
        0x00,
        0x00,
        0x02,
        0x71,
        0x00,
        0x0C,
        0x9C,
        0x00,
        0x01,
        0x24,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x01,
        0x00,
        0x02,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x30,
        0x00,
        0xD2,
        0x60,
        0x60,
        0x00,
        0xC0,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x0D,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };

    const uint8_t firstRegister = 0;
    i2c_write_blocking(i2c1, I2C1_ADDR, &firstRegister, 1, true);
    i2c_write_blocking(i2c1, I2C1_ADDR, registerMap.data(), registerMap.size(), false);

    // PLLA and PLLb soft reset
    uint8_t addr = 177;
    uint8_t data = 0xAC;
    i2c_write_blocking(i2c1, I2C1_ADDR, &addr, 1, true);
    i2c_write_blocking(i2c1, I2C1_ADDR, &data, 1, false);

    // enable output 0
    addr = 3;
    data = 0xFE;
    i2c_write_blocking(i2c1, I2C1_ADDR, &addr, 1, true);
    i2c_write_blocking(i2c1, I2C1_ADDR, &data, 1, false);
}

uint32_t Si5351::pllFrequency(uint32_t frequency)
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

    return (frequency + 21600000) + 5;
}

void Si5351::writePLL(const uint8_t *values)
{
}
