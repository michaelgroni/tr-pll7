#include "Si5351.h"

#include "settings.h"
#include "I2Cinput.h"
#include "GPIOinput.h"

#include "hardware/i2c.h"

#include <numeric>

const uint8_t I2C1_ADDR = 0x60;

const double XTAL_FREQ = 25000000;  // Referenzfrequenz (25 MHz)
const double VCO_FREQ = 730560000; // VCO-Frequenz (fixiert)

Si5351 *Si5351::getInstance()
{
	static Si5351 instance;
	return &instance;
}

void Si5351::write(const uint32_t frequency)
{
	auto newF = pllFrequency(frequency);

	if (pllFreq != newF)
	{
		pllFreq = newF;

		uint8_t allOutputsOff[2] = {3, 0xFF};
		i2c_write_blocking(i2c1, I2C1_ADDR, allOutputsOff, 2, false);

		// Multisynth-Divider (MS)
		double msDivider = VCO_FREQ / pllFreq;
		uint32_t msInt, msNum, msDenom;
		calculateDivider(msDivider, msInt, msNum, msDenom);
		setMultisynthDivider(0, msInt, msNum, msDenom);

		uint8_t pllSoftReset[2] = {177, 0xA0};
		i2c_write_blocking(i2c1, I2C1_ADDR, pllSoftReset, 2, false);
		uint8_t enableOutput0[2] = {3, 0xFE};
		i2c_write_blocking(i2c1, I2C1_ADDR, enableOutput0, 2, false);
	}


}

Si5351::Si5351()
{
	i2c_init(i2c1, I2C_CLOCK);
	gpio_set_function(PLL_I2C1_SDA, GPIO_FUNC_I2C);
	gpio_set_function(PLL_I2C1_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(PLL_I2C1_SDA);
	gpio_pull_up(PLL_I2C1_SCL);

	// wait until Si5351 is initialized and CLKIN is valid
	uint8_t siRegister = 0;
	uint8_t siData;
	do
	{
		i2c_write_blocking(i2c1, I2C1_ADDR, &siRegister, 1, false);
		i2c_read_blocking(i2c1, I2C1_ADDR, &siData, 1, false);
	} while ((siData & 0x88) != 0);

	uint8_t allOutputsOff[2] = {3, 0xFF};
	i2c_write_blocking(i2c1, I2C1_ADDR, allOutputsOff, 2, false);

	uint8_t disableOEBcontrol[2] = {9, 0xFF};
	i2c_write_blocking(i2c1, I2C1_ADDR, disableOEBcontrol, 2, false);
	
	uint8_t outputDriverDown[2] = {16, 0x8C};
	for (outputDriverDown[0] = 16; outputDriverDown[0] <= 23; outputDriverDown[0]++)
	{
		i2c_write_blocking(i2c1, I2C1_ADDR, outputDriverDown, 2, false);
	}

	// write register map
	i2c_write_blocking(i2c1, I2C1_ADDR, registerMap1.data(), registerMap1.size(), false);
	i2c_write_blocking(i2c1, I2C1_ADDR, registerMap2.data(), registerMap2.size(), false);


	// PLL	
	double feedbackDivider = VCO_FREQ / XTAL_FREQ;

	uint32_t integerPart, numerator, denominator;
	calculateDivider(feedbackDivider, integerPart, numerator, denominator);

	setPLL(0, integerPart, numerator, denominator);
	

	uint8_t pllSoftReset[2] = {177, 0xA0};
	i2c_write_blocking(i2c1, I2C1_ADDR, pllSoftReset, 2, false);
    

	uint8_t enableOutput0[2] = {3, 0xFE};
	i2c_write_blocking(i2c1, I2C1_ADDR, enableOutput0, 2, false);
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

	return (frequency + 21600000) / 5;
}

void Si5351::calculateDivider(const double value, uint32_t &integerPart, uint32_t &numerator, uint32_t &denominator)
{
	integerPart = static_cast<uint32_t>(value);
	double fractional = value - integerPart;

	denominator = 1'000'000;
	numerator = static_cast<uint32_t>(fractional * denominator);

	uint32_t gcd = std::gcd(numerator, denominator);
	numerator /= gcd;
	denominator /= gcd;
}

void Si5351::setMultisynthDivider(uint8_t channel, uint32_t mult, uint32_t num, uint32_t denom)
{
	uint8_t msRegisters[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

	msRegisters[0] = 42 + (8 * channel);			// first register to write

	uint16_t p1 = 128 * mult + (128 * num / denom) - 512;
	uint16_t p2 = 128 * num - denom * (128 * num/denom);	

	msRegisters[1]  = ((denom >> 8) & 0xFF);	// 42
	msRegisters[2]  = (denom & 0xFF);		  	// 43
	msRegisters[3]  = ((p1 >> 16) & 0x03);		// 44
	msRegisters[4]  = ((p1) >> 8 & 0xFF);		// 45
	msRegisters[5]  = (p1 & 0xFF);		  		// 46

	msRegisters[6]  = ((denom >> 12) & 0xF0);	// 47
	msRegisters[6] += ((p2 >> 16) & 0x0F);		// 47

	msRegisters[7] = ((p2 >> 8) & 0xFF);	    // 48
	msRegisters[8] = (p2 & 0xFF);		    	// 49

	i2c_write_blocking(i2c1, I2C1_ADDR, msRegisters, sizeof(msRegisters), false);
}

void Si5351::setPLL(uint8_t pll, uint32_t mult, uint32_t num, uint32_t denom)
{
    uint8_t pllRegisters[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

	pllRegisters[0] = (pll == 0) ? 26 : 34; // first address 26 for PLLA, 34 for PLLB

	uint16_t p1 = 128 * mult + (128 * num / denom) - 512;
	uint16_t p2 = 128 * num - denom * (128 * num/denom);

	// Calculate PLL configuration registers
	pllRegisters[1]  = ((denom >> 8) & 0xFF);   // P3 PLL_DEN[15:8]
	pllRegisters[2]  = (denom & 0xFF);          // P3 PLL_DEN[7:0]
	pllRegisters[3]  = p1 >> 16;				// P1[17-16]
	pllRegisters[4]  = (p1 >> 8) & 0xFF;		// P1[15-8]
	pllRegisters[5]  = (p1 & 0xFF);			    // P1[7-0]

	pllRegisters[6]  = ((denom >> 12) & 0xF0);  // P3 PLL_DEN[19:16]
    pllRegisters[6] += p2 >> 16;				// P2[17-16]

	pllRegisters[7]  = ((p2 >> 8) & 0xFF);      // P2[15:8]
	pllRegisters[8] =  (p2 & 0xFF);             // P2[7:0]
    
    pllRegisters[8] = 0x00;                    // Phase Offset (set to default: 0)
 
    i2c_write_blocking(i2c1, I2C1_ADDR, pllRegisters, sizeof(pllRegisters), false);
}