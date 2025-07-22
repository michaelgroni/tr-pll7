#pragma once

#include "pico-ssd1306/ssd1306.h"

#include "settings.h"
#include "TrxState.h"
#include "Scanner.h"

#include <string>

class Display
{
public:
    static Display* getInstance();
    void update(TrxState& trxState, const Scanner& scanner);

private:
    uint32_t frequency = 0;
    std::string line2 = "";
    std::string line3 = "";
    char infoNortheast = ' ';

    Display();                              
    Display(const Display&);                   // disable copy constructor              
    Display & operator = (const Display &);    // disable operator =

    pico_ssd1306::SSD1306 oled = pico_ssd1306::SSD1306(I2C_PORT, OLED_ADDRESS, pico_ssd1306::Size::W128xH32);

    void drawDigit(uint8_t x, uint8_t y, uint digit);
    void setFrequency(const uint32_t frequency);
    void setLine2(const std::string line2);
    void setLine3(const std::string line2);
    void setInfoNortheast(const char c);
};