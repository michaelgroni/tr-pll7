#include "GPIOinput.h"

#include "GPIOoutput.h"

bool isPressed(const uint8_t gpio)
{
    return !gpio_get(gpio);
}


bool wasPressed(const uint8_t gpio)
{
    const int32_t mask = 0xF << 4 * (gpio % 8);
    const uint32_t events = (io_bank0_hw->intr[gpio / 8] & mask) >> 4 * ( gpio % 8);
    if (events & GPIO_IRQ_EDGE_FALL)
    {
        gpio_acknowledge_irq(gpio, GPIO_IRQ_EDGE_FALL); // clear fall event
        return true;
    }
    else
    {
        return false;
    }
}


bool isSquelchOpen()
{
    return gpio_get(GPIO_SQUELCH_OPEN);
}


int readRotaryEncoder(uint rotarySm)
{
    int updown = 0;
    while (!pio_sm_is_rx_fifo_empty(ROTARY_PIO, rotarySm))
    {
        updown += (int32_t) pio_sm_get(ROTARY_PIO, rotarySm);
    }
    return updown;
}

