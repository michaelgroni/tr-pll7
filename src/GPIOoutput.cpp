#include "GPIOoutput.h"

#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"

#include "settings.h"
#include "beep.pio.h"
#include "ctcss.pio.h"


Piezo* Piezo::getInstance()
{
    static Piezo instance;
    return &instance;
}

Piezo::Piezo()
{
    // setup beep pio
    const uint beepOffset = pio_add_program(BEEP_PIO, &beep_program);
    beepSm = pio_claim_unused_sm(BEEP_PIO, true);
    pio_sm_config beepConfig = beep_program_get_default_config(beepOffset);
    sm_config_set_set_pins(&beepConfig, BEEP_PIN, 1);
    pio_gpio_init(BEEP_PIO, BEEP_PIN);
    pio_sm_init(BEEP_PIO, beepSm, beepOffset, &beepConfig);
    pio_sm_set_enabled(BEEP_PIO, beepSm, false);     
}

void Piezo::beepOK()
{
    pio_sm_set_clkdiv(BEEP_PIO, beepSm, 128);
    pio_sm_set_enabled(BEEP_PIO, beepSm, true);
    sleep_ms(75);
    pio_sm_set_enabled(BEEP_PIO, beepSm, false);
}


void Piezo::beepWriteOK()
{
    pio_sm_set_clkdiv(BEEP_PIO, beepSm, 128);
    pio_sm_set_enabled(BEEP_PIO, beepSm, true);
    sleep_ms(250);
    pio_sm_set_enabled(BEEP_PIO, beepSm, false);
}

    
void Piezo::beepError()
{
    pio_sm_set_clkdiv(BEEP_PIO, beepSm, 255);
    for (int i=0; i<4; i++)
    {
        pio_sm_set_enabled(BEEP_PIO, beepSm, true);
        sleep_ms(50);
        pio_sm_set_enabled(BEEP_PIO, beepSm, false);
        sleep_ms(50);
    }
}
    

Ctcss* Ctcss::getInstance()
{
    static Ctcss instance;
    return &instance;
}

Ctcss::Ctcss()
{
    // setup ctcss pio
    const uint ctcssOffset = pio_add_program(CTCSS_PIO, &ctcss_program);
    ctcssSm = pio_claim_unused_sm(CTCSS_PIO, true);
    pio_sm_config ctcssConfig = ctcss_program_get_default_config(ctcssOffset);
    sm_config_set_set_pins(&ctcssConfig, CTCSS_PIN, 1);
    pio_gpio_init(CTCSS_PIO, CTCSS_PIN);
    pio_sm_init(CTCSS_PIO, ctcssSm, ctcssOffset, &ctcssConfig);
    pio_sm_set_enabled(CTCSS_PIO, ctcssSm, false);  
}

void Ctcss::update(TrxState &trxState)
{
    if (!trxState.isCtcssOn())
    {
        if (on)
        {
            setOff();
        }
    }
    else // CTCSS on
    {
        double frequency = trxState.getCtcssFrequency();
        if ((frequency != this->frequency) || !on)
        {
            setOn(frequency);
        }
    }
}

void Ctcss::setOn(const float frequency)
{
    const auto sysClock = clock_get_hz(clk_sys);
    constexpr uint cycles = 180;
    const float clkDiv = sysClock / (frequency * cycles);
    pio_sm_set_clkdiv(CTCSS_PIO, ctcssSm, clkDiv);
    pio_sm_set_enabled(CTCSS_PIO, ctcssSm, true);  
    this->frequency = frequency;
    on = true;
}

void Ctcss::setOff()
{
    pio_sm_set_enabled(CTCSS_PIO, ctcssSm, false);
    on = false;
}