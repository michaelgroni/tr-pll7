#include "Scanner.h"

#include "hardware/timer.h"

#include "GPIOinput.h"
#include "GPIOoutput.h"
#include "I2Cinput.h"
#include "memory.h"
#include "TrxStateMemories.h"

void Scanner::update(TrxState *trxState)
{
    sleep_ms(SCANNER_WAIT_PLL_TIME); // wait for the pll

    if (wasOpen)
    {
        sleep_ms(SCANNER_WAIT_SQUELCH_TIME); // wait for the squelch to close
        wasOpen = false;
    }

    if (isSquelchOpen())
    {
        Piezo::getInstance()->beepOK();
        wasOpen = true;
        for (int i = 0; (i < 10) && isOn(); i++)
        {
            sleep_ms(SCANNER_WAIT_QSO_TIME / 10);
            if ((trxState->getI2Cinput()).isPressedPtt() || wasPressed(GPIO_ROTARY_BUTTON))
            {
                Piezo::getInstance()->beepOK();
                setOn(false);
            }
        }
    }

    if (isOn())
    {
        TrxStateVfo *tsv = dynamic_cast<TrxStateVfo *>(trxState);
        TrxStateMemories *tsm = dynamic_cast<TrxStateMemories *>(trxState);

        if (tsv != nullptr) // vfo mode
        {

            if (up)
            {
                if (trxState->getRxFrequency() >= scanMax())
                {
                    tsv->setRxFrequency(scanMin());
                }
                else
                {
                    trxState->up(1);
                }
            }
            else
            {
                if (trxState->getRxFrequency() <= scanMin())
                {
                    tsv->setRxFrequency(scanMax());
                }
                else
                {
                    trxState->up(-1);
                }
            }
        }
        else if (tsm != nullptr) // memory mode
        {
            const int direction = up ? 1 : -1;
            tsm->up(direction);
        }
    }
}

void Scanner::setUp(const bool up)
{
    this->up = up;
}

void Scanner::setOn(const bool on)
{
    this->on = on;
}

bool Scanner::isOn() const
{
    return on;
}