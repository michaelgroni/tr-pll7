#pragma once

#include "pico/stdlib.h"
#include "rp2040-Si5351/Si5351.hpp"

void setupI2C();
void setupGPIOinput();
uint setupPTTpio();
uint setupRotaryPio();
