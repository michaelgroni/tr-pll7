#pragma once

#include <unordered_map>
#include <string>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/pio.h"
#include "hardware/flash.h"


// memory
inline const uint32_t MY_FLASH_OFFSET = 510 * FLASH_SECTOR_SIZE;
inline const uint32_t MY_FLASH_DATA = MY_FLASH_OFFSET + FLASH_SECTOR_SIZE;
inline const size_t MEMORIES = 100;


// frequencies
inline const uint32_t F_MIN = 400000000;
inline const uint32_t F_MAX = 450000000;
inline const uint32_t F_MIN_TX = 430000000;
inline const uint32_t F_MAX_TX = 440000000;
inline const uint32_t VFO_A_INIT = 435000000;
inline const uint32_t VFO_B_INIT = 438775000;

// ADC
inline const unsigned int F_SAMPLE = 20000;

// waiting times in milliseconds
inline const uint32_t MAIN_LOOP_PAUSE_TIME = 100;
inline const uint32_t SCANNER_WAIT_QSO_TIME = 4000;
inline const uint32_t SCANNER_WAIT_PLL_TIME = 5; // wait for PLL and squelch before checking the scanner stop signal
inline const uint32_t SCANNER_WAIT_SQUELCH_TIME = 70;

// I2C bus
inline const uint I2C_CLOCK = 100000;
inline const auto I2C_PORT = i2c0;

// I2C OLED display 128 x 32
inline const int OLED_ADDRESS = 0x3C;
inline const bool OLED_FLIPPED = true;
inline const unsigned char OLED_CONTRAST = 255;

// I2C input / output
inline const uint8_t ENCODER_IC1_ADDR {0x20};
inline const uint8_t ENCODER_IC2_ADDR {0x21};
inline const uint8_t CONTROL_IC1_ADDR {0x24}; // PCF8574 0x26; PCF8574A 0x3E; PCB v6 0x24 only PCF8574 possible

// PIOs
inline const auto ROTARY_PIO = pio0;
inline const auto BEEP_PIO = pio0;
inline const auto CTCSS_PIO = pio1;
inline const auto PTT_PIO = pio1; // origin 0
inline const auto R2R_PIO = pio1;

// PTT pio pins
inline const uint PTT_OUT_PIN = 9;
inline const uint PTT_IN_PIN = 8;

// GPIO input pins
inline const std::unordered_map<std::string, uint> gpioInPins =
{
    {"rotaryButton", 22},
    {"micUp", 2},
    {"micDown", 3},
    {"squelchOpen", 18}
};

// rotary encoder pio input pins
inline const uint8_t ROTARY_CLOCK = 21;
inline const uint8_t ROTARY_DATA = 19;

// pio output pins
inline const uint BEEP_PIN = 5;
inline const uint CTCSS_PIN = 20;

// DAC output
inline const size_t R2R_SIZE = 8;
inline const uint32_t R2R_MASK = 0xFF000;
inline const uint_fast8_t R2R_BASE_PIN = 10;
