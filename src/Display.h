#pragma once

#include "pico-ssd1306/ssd1306.h"

#include "TrxState.h"
#include "Scanner.h"

#include <string>
#include <memory>
class Display
{
public:
    Display(i2c_inst* i2cPort);
    void update(TrxState& trxState, const Scanner& scanner);

private:
    uint32_t frequency = 0;
    std::string line2 = "";
    std::string line3 = "";
    char infoNortheast = ' ';                        

    std::unique_ptr<pico_ssd1306::SSD1306> oled;

    void drawDigit(uint8_t x, uint8_t y, uint digit);
    void setFrequency(const uint32_t frequency);
    void setLine2(const std::string line2);
    void setLine3(const std::string line2);
    void setInfoNortheast(const char c);
};