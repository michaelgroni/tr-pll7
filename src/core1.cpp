#include "core1.h"

#include <memory>

#include "hardware/adc.h"

#include "settings.h"
#include "TrxStateSpecialMemoryFIR.h"
#include "FirLowpass.h"
#include "FirNotch.h"
#include "Median.h"
#include "r2r.pio.h"


void setup_adc(const unsigned int f_sample)
{
	adc_init();
	adc_gpio_init(26);
	adc_select_input(0);
}


static FirLowpass bandpass(2700, F_SAMPLE); // TODO fix globals
static FirNotch notch = FirNotch(1000, F_SAMPLE);
static int r2rSm;


int stateMachineR2R;
bool timerHandler(struct repeating_timer* t)
{
	static uint16_t sample;
	sample = adc_read();
	// bandpass.filter(sample);
	pio_sm_put(R2R_PIO, stateMachineR2R, (uint32_t) sample >> 4);
	return true;
}

void core1_entry()
{
	// setup R2R PIO
	uint offsetR2R = pio_add_program(R2R_PIO, &r2r_program);
	stateMachineR2R = pio_claim_unused_sm(R2R_PIO, true);
	pio_sm_config r2rConfig = r2r_program_get_default_config(offsetR2R);
	sm_config_set_out_pins(&r2rConfig, R2R_BASE_PIN, R2R_SIZE);
	sm_config_set_out_shift(&r2rConfig, true, true, 32);
	for (uint pin = R2R_BASE_PIN; pin < R2R_BASE_PIN + R2R_SIZE; pin++)
	{
    	pio_gpio_init(R2R_PIO, pin);
    	gpio_set_slew_rate(pin, GPIO_SLEW_RATE_FAST);
	}
	pio_sm_init(R2R_PIO, stateMachineR2R, offsetR2R, &r2rConfig);
	pio_sm_set_consecutive_pindirs(R2R_PIO, stateMachineR2R, R2R_BASE_PIN, R2R_SIZE, true);
	pio_sm_set_enabled(R2R_PIO, stateMachineR2R, true);
	

	setup_adc(F_SAMPLE);


	filterConfig fc;
	queue_remove_blocking(&filterConfigQueue, &fc);

	// bandpass = FirBandpass(fc.fLow, fc.fHigh, F_SAMPLE);
	// notch = FirNotch(1000, F_SAMPLE);
	Median median = Median(fc.medianSize);

	repeating_timer_t timer;
	auto pool = alarm_pool_create_with_unused_hardware_alarm(2);
	alarm_pool_add_repeating_timer_us (pool, 1000000/F_SAMPLE, timerHandler, nullptr, &timer);

	while (true)
	{
		sleep_ms(MAIN_LOOP_PAUSE_TIME);
		
		/*
		auto oldFc = fc;

		if (queue_try_remove(&filterConfigQueue, &fc))
		{
			adc_run(false);

			if ((fc.fLow != oldFc.fLow) || (fc.fHigh != oldFc.fHigh))
			{
				bandpass.set(fc.fLow, fc.fHigh, F_SAMPLE);
			}

			if (fc.fNotch != oldFc.fNotch)
			{
				notch.set(fc.fNotch, F_SAMPLE);
			}

			if (fc.medianSize != oldFc.medianSize)
			{
				median.setSize(fc.medianSize);
			}

			adc_run(true);
		}
		*/
	}
}
