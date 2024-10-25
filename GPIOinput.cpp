#include "GPIOinput.h"

#include "GPIOoutput.h"

bool isPressed(std::string pinName)
{
    auto pin = gpioInPins.at(pinName);
    return !gpio_get(pin);
}


bool wasPressed(std::string pinName)
{
    auto pin = gpioInPins.at(pinName);
    int32_t mask = 0xF << 4 * (pin % 8);
    uint32_t events = (iobank0_hw->intr[pin / 8] & mask) >> 4 * ( pin % 8);
    if (events & GPIO_IRQ_EDGE_FALL)
    {
        gpio_acknowledge_irq(pin, GPIO_IRQ_EDGE_FALL); // clear fall event
        return true;
    }
    else
    {
        return false;
    }
}


bool isSquelchOpen()
{
    auto pin = gpioInPins.at("squelchOpen");
    return gpio_get(pin);
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

int_fast8_t readUpDownButtons()
{
    if (wasPressed("micUp") && isPressed("micUp")) // read event and state to avoid crosstalk effects
    {
        Piezo::getInstance()->beepOK();
        return 1;
    }
    else if (wasPressed("micDown") && isPressed("micDown"))
    {
        Piezo::getInstance()->beepOK();
        return -1;
    } 
    else
    {
        return 0;
    }
}
