#pragma once

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "rp2040-Si5351/Si5351.hpp"

void setupI2C(i2c_inst_t* i2cIO, i2c_inst_t* i2cSi5351A);
void setupGPIOinput();
uint setupPTTpio();
uint setupRotaryPio();
