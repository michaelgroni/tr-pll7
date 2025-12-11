#include "setup.h"
#include "settings.h"
#include "hardware/pio.h"
#include "ptt.pio.h"
#include "rotaryEncoder.pio.h"

const uint8_t I2C_IO_SDA = 0;
const uint8_t I2C_IO_SCL = 1;

const uint8_t I2C_SI5351A_SDA = 2;
const uint8_t I2C_SI5351A_SCL = 3;

const uint I2C_IO_CLOCK = 100000;
const uint I2C_SI5351A_CLOCK = 400000;


void setupI2C(i2c_inst_t* i2cIO, i2c_inst_t* i2cSi5351A)
{
    gpio_pull_up(I2C_IO_SDA);
    gpio_pull_up(I2C_IO_SCL);
    i2c_init(i2cIO, I2C_IO_CLOCK);
    gpio_set_function(I2C_IO_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_IO_SCL, GPIO_FUNC_I2C);

    gpio_pull_up(I2C_SI5351A_SDA);
    gpio_pull_up(I2C_SI5351A_SCL);    
    i2c_init(i2cSi5351A, I2C_SI5351A_CLOCK);
    gpio_set_function(I2C_SI5351A_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SI5351A_SCL, GPIO_FUNC_I2C);
}

void setupGPIOinput()
{
    for (auto [name, pin] : gpioInPins)
    {
        gpio_init(pin);
        gpio_pull_up(pin);
    }
}

uint setupPTTpio()
{
    // PTT_IN_PIN setup see also GPIO input
    uint pttOffset = pio_add_program(PTT_PIO, &ptt_program);
    uint pttSm = pio_claim_unused_sm(PTT_PIO, true);
    pio_sm_config pttConfig = ptt_program_get_default_config(pttOffset);
    pio_gpio_init(PTT_PIO, PTT_IN_PIN);
    gpio_pull_up(PTT_IN_PIN);
    pio_gpio_init(PTT_PIO, PTT_OUT_PIN);
    sm_config_set_jmp_pin(&pttConfig, PTT_IN_PIN);
    sm_config_set_set_pins(&pttConfig, PTT_OUT_PIN, 1);
    pio_sm_init(PTT_PIO, pttSm, pttOffset, &pttConfig);
    pio_sm_set_enabled(PTT_PIO, pttSm, true);
    return pttSm;
}

uint setupRotaryPio()
{
    gpio_pull_up(ROTARY_CLOCK);
    gpio_pull_up(ROTARY_DATA);
    uint rotaryOffset = pio_add_program(ROTARY_PIO, &rotary_program);
    uint rotarySm = pio_claim_unused_sm(ROTARY_PIO, true);
    pio_sm_config rotaryConfig = rotary_program_get_default_config(rotaryOffset);
    pio_gpio_init(ROTARY_PIO, ROTARY_CLOCK);
    pio_gpio_init(ROTARY_PIO, ROTARY_DATA);
    sm_config_set_in_pins(&rotaryConfig, ROTARY_CLOCK);
    sm_config_set_jmp_pin(&rotaryConfig, ROTARY_DATA);
    sm_config_set_fifo_join(&rotaryConfig, PIO_FIFO_JOIN_RX);
    sm_config_set_in_shift(&rotaryConfig, false, false, 0);
    pio_sm_init(ROTARY_PIO, rotarySm, rotaryOffset, &rotaryConfig);
    pio_sm_set_enabled(ROTARY_PIO, rotarySm, true);
    return rotarySm;
}
