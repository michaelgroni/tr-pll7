#pragma once

#include "settings.h"

#include <string>

bool isPressed(const uint8_t gpio);
bool wasPressed(const uint8_t gpio);
bool isSquelchOpen();
int readRotaryEncoder(uint rotarySm);

