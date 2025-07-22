#pragma once

#include "settings.h"

#include <string>

bool isPressed(std::string pinName);
bool wasPressed(std::string pinName);
bool isSquelchOpen();
int readRotaryEncoder(uint rotarySm);
int_fast8_t readUpDownButtons();

