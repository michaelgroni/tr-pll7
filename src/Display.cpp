#include "Display.h"
#include "I2Cinput.h"
#include "TrxStateVfo.h"
#include "TrxStateSpecialMemory.h"
#include "TrxStateScanMin.h"
#include "TrxStateScanMax.h"
#include "TrxStateMemories.h"
#include "TrxStateSpecialMemoryFIR.h"
#include "pico-ssd1306/textRenderer/TextRenderer.h"
#include "pico-ssd1306/shapeRenderer/ShapeRenderer.h"
#include "settings.h"

#include <iomanip>
#include <sstream>

Display::Display(i2c_inst* i2cPort)
:oled(i2cPort, OLED_ADDRESS, pico_ssd1306::Size::W128xH32)
{
    oled.setOrientation(OLED_FLIPPED);
    oled.setContrast(OLED_CONTRAST);
}

void Display::update(TrxState &trxState, const Scanner &scanner)
{
    const TrxStateScanMin *tsmin = dynamic_cast<TrxStateScanMin *>(&trxState);
    const TrxStateScanMax *tsmax = dynamic_cast<TrxStateScanMax *>(&trxState);
    const TrxStateMemories *tsmem = dynamic_cast<TrxStateMemories *>(&trxState);
    const TrxStateVfo *tsv = dynamic_cast<TrxStateVfo *>(&trxState);
    const TrxStateSpecialMemoryFIR *tsfir = dynamic_cast<TrxStateSpecialMemoryFIR *>(&trxState);

    // frequency
    setFrequency(trxState.getCurrentFrequency());

    // info northeast (one char)
    const auto txf = trxState.getTxFrequency();
    const auto rxf = trxState.getRxFrequency();
    char infoNortheast;

    if (txf < rxf)
    {
        infoNortheast = '-';
    }
    else if (txf > rxf)
    {
        infoNortheast = '+';
    }
    else // no duplex shift
    {
        if ((tsmin != nullptr) && tsmin->isNotSaved() || (tsmax != nullptr) && tsmax->isNotSaved())
        {
            infoNortheast = '*';
        }
        else
        {
            infoNortheast = ' ';
        }
    }

    setInfoNortheast(infoNortheast);

    // line2
    std::string newLine2;
    if (tsfir != nullptr)
    {
        auto info = tsfir->getInfo();
        std::stringstream s;
        s << info.at(0) << ' ' << info.at(1);
        newLine2 = s.str();
    }
    else if (tsmem != nullptr && tsmem->isWriteModeOn())
    {
        newLine2 = "save M / del REV";
    }
    else if (!trxState.isCtcssOn()) // no CTCSS; step in line 2
    {
        if (tsv != nullptr || tsmin != nullptr || tsmax != nullptr)
        {
            newLine2 = "Step " + trxState.getStepToString();
        }
        else
        {
            newLine2 = "";
        }
    }
    else // CTCSS
    {
        std::stringstream s;
        s << std::fixed << std::setprecision(1) << trxState.getCtcssFrequency();
        newLine2 = "CTCSS " + s.str();
    }

    setLine2(newLine2);
    
    // line 3
    std::string newLine3 = "";
    if (tsfir != nullptr)
    {
        auto info = tsfir->getInfo();
        newLine3 = info.at(2);
    }
    else if (tsmin != nullptr)
    {
        newLine3 = "scan min";
    }
    else if (tsmax != nullptr)
    {
        newLine3 = "scan max";
    }
    else if (tsmem != nullptr) // memory mode
    {
        std::stringstream s;
        s << tsmem->getMemoryIndex();

        if (scanner.isOn())
        {
            newLine3 = "Mem scan " + s.str();
        }
        else
        {
            newLine3 = "Mem " + s.str();
        }
    }
    else if (tsv != nullptr) // VFO
    {
        newLine3 = mode2String( trxState.getI2Cinput().getMode() );
    }
    else if (scanner.isOn())
    {
        newLine3 = "scan";
    }
    setLine3(newLine3);

    oled.sendBuffer();
}

/*
    digits 0..9
    height 14, width 8
*/
void Display::drawDigit(const uint8_t x, const uint8_t y, const uint digit)
{
    switch (digit)
    {
    case 1:
        drawLine(&oled, x + 5, y, x + 5, y + 14);
        break;
    case 2:
        drawLine(&oled, x + 1, y, x + 7, y);
        drawLine(&oled, x + 8, y, x + 8, y + 6);
        drawLine(&oled, x + 1, y + 7, x + 7, y + 7);
        drawLine(&oled, x, y + 8, x, y + 13);
        drawLine(&oled, x + 1, y + 14, x + 7, y + 14);
        break;
    case 3:
        drawLine(&oled, x + 1, y, x + 7, y);
        drawLine(&oled, x + 8, y, x + 8, y + 14);
        drawLine(&oled, x + 1, y + 7, x + 7, y + 7);
        drawLine(&oled, x + 1, y + 14, x + 7, y + 14);
        break;
    case 4:
        drawLine(&oled, x, y, x, y + 6);
        drawLine(&oled, x + 8, y, x + 8, y + 14);
        drawLine(&oled, x + 1, y + 7, x + 7, y + 7);
        break;
    case 6:
        drawLine(&oled, x, y + 7, x, y + 14);
    case 5:
        drawLine(&oled, x, y, x, y + 6);
        drawLine(&oled, x + 1, y, x + 7, y);
        drawLine(&oled, x + 8, y + 7, x + 8, y + 14);
        drawLine(&oled, x + 1, y + 7, x + 7, y + 7);
        drawLine(&oled, x + 1, y + 14, x + 7, y + 14);
        break;
    case 0:
        drawLine(&oled, x, y, x, y + 14);
        drawLine(&oled, x + 1, y + 14, x + 7, y + 14);
    case 7:
        drawLine(&oled, x + 1, y, x + 7, y);
        drawLine(&oled, x + 8, y, x + 8, y + 14);
        break;
    case 8:
        drawLine(&oled, x, y + 7, x, y + 14);
    case 9:
        drawLine(&oled, x, y, x, y + 6);
        drawLine(&oled, x + 1, y, x + 7, y);
        drawLine(&oled, x + 8, y, x + 8, y + 14);
        drawLine(&oled, x + 1, y + 7, x + 7, y + 7);
        drawLine(&oled, x + 1, y + 14, x + 7, y + 14);
        break;
    default: // unknown character
        drawLine(&oled, x + 1, y, x + 6, y);
        drawLine(&oled, x + 1, y + 7, x + 6, y + 14);
        drawLine(&oled, x + 1, y + 14, x + 6, y + 14);
    }
}

void Display::setFrequency(const uint32_t frequency)
{
    this->frequency = frequency;

    fillRect(&oled, 0, 0, 110, 15, pico_ssd1306::WriteMode::SUBTRACT); // overwrite old content

    uint8_t x = 0;
    uint column = 0;
    for (uint i = 1000000000; i >= 10; i /= 10)
    {
        const uint digit = (frequency % i) / (i / 10);
        drawDigit(x, 0, digit);

        if (column == 2 || column == 5) // thousands point
        {
            oled.setPixel(x + 11, 14);
            x += 14;
        }
        else // no thousands point
        {
            x += 12;
        }

        column++;
    }
}

void Display::setLine2(const std::string line2)
{
    this->line2 = line2;

    fillRect(&oled, 0, 17, 128, 24, pico_ssd1306::WriteMode::SUBTRACT); // overwrite old content
    drawText(&oled, font_8x8, line2.c_str(), 0, 17);
}

void Display::setLine3(const std::string line3)
{
    this->line3 = line3;

    fillRect(&oled, 0, 25, 128, 32, pico_ssd1306::WriteMode::SUBTRACT); // overwrite old content
    drawText(&oled, font_8x8, line3.c_str(), 0, 25);
}

void Display::setInfoNortheast(const char c)
{
    this->infoNortheast = c;

    char cString[2];
    cString[0] = c;
    cString[1] = '\0';

    fillRect(&oled, 117, 0, 127, 15, pico_ssd1306::WriteMode::SUBTRACT); // overwrite old content
    drawText(&oled, font_12x16, cString, 117, 0);
}
