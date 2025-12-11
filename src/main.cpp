#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"

#include "pico-ssd1306/ssd1306.h"
#include "rp2040-Si5351/Si5351.hpp"

#include "settings.h"
#include "TrxStateVfo.h"
#include "TrxStateScanMin.h"
#include "TrxStateScanMax.h"
#include "TrxStateMemories.h"
#include "TrxStateSpecialMemoryFIR.h"
#include "Display.h"
#include "I2Cinput.h"
#include "GPIOinput.h"
#include "GPIOoutput.h"
#include "ADF4351.h"
#include "Scanner.h"
#include "core1.h"
#include "setup.h"
#include "ptt.pio.h"

// I2C bus
inline const auto I2C_PORT_IO = i2c0;
inline const auto I2C_PORT_SI5351A = i2c1;

void setTxAllowed(const bool allowed, const uint pttSm);

int main()
{
    setupI2C(I2C_PORT_IO, I2C_PORT_SI5351A);
    setupGPIOinput();
    auto pttSm = setupPTTpio();
    auto rotarySm = setupRotaryPio();

    auto i2cInput = I2Cinput::getInstance();

    TrxStateVfo vfoA(VFO_A_INIT);
    TrxStateVfo vfoB(VFO_B_INIT);

    TrxStateMemories memories;

    Scanner scanner;

    setTxAllowed(false, pttSm);

    TrxState *currentState = I2Cinput::getInstance()->isPressedAB() ? &vfoB : &vfoA;

    TrxStateSpecialMemoryFIR stateFir(currentState);

    Piezo::getInstance()->beepOK();

    flashInit();

    auto firConfig = stateFir.getConfig();
    queue_init(&filterConfigQueue, sizeof(filterConfig), 2);
    queue_add_blocking(&filterConfigQueue, &firConfig);

    multicore_launch_core1(core1_entry);

    // main loop
    while (true)
    {
        sleep_ms(MAIN_LOOP_PAUSE_TIME);

        // read I²C input
        i2cInput->update(); // must be called in the main loop

        // select state instance
        switch (I2Cinput::getInstance()->getSpecialMemoryChannel())
        {
        case 1: // special memory scan min
            currentState = &trxStateScanMin;
            break;
        case 2: // special memory scan max
            currentState = &trxStateScanMax;
            break;
        case 3: // special memory filter
            if (currentState != &stateFir)
            {
                stateFir.update(currentState);
                currentState = (TrxState *)&stateFir;
            }
            break;
        default: // no special memory channel active
            if (I2Cinput::getInstance()->isPressedMR())
            {
                currentState = &memories;
            }
            else
            {
                currentState = I2Cinput::getInstance()->isPressedAB() ? &vfoB : &vfoA;
            }
        }

        // read rotary encoder and up/down buttons
        int updown = readRotaryEncoder(rotarySm);
        updown += readUpDownButtons();

        // read step buttons
        if (I2Cinput::getInstance()->wasPressedStepIncrease())
        {
            currentState->stepUp();
            Piezo::getInstance()->beepOK();
        }
        if (I2Cinput::getInstance()->wasPressedStepDecrease())
        {
            currentState->stepDown();
            Piezo::getInstance()->beepOK();
        }

        auto mode = I2Cinput::getInstance()->getMode();

        switch (I2Cinput::getInstance()->getSpecialMemoryChannel())
        {
        case 1: // special memory scan min
            if (updown != 0)
            {
                currentState->up(updown);
            }

            if (I2Cinput::getInstance()->wasPressedM())
            {
                Piezo::getInstance()->beepOK();
                trxStateScanMin.save();
            }
            break;
        case 2: // special memory scan max
            if (updown != 0)
            {
                currentState->up(updown);
            }

            if (I2Cinput::getInstance()->wasPressedM())
            {
                Piezo::getInstance()->beepOK();
                trxStateScanMax.save();
            }
            break;
        case 3: // special memory filter
            if (wasPressed("rotaryButton") && isPressed("rotaryButton"))
            {
                Piezo::getInstance()->beepError();
            }

            stateFir.up(updown);
            if (stateFir.wasChanged())
            {
                firConfig = stateFir.getConfig();
                // queue_add_blocking(&filterConfigQueue, &firConfig);
            }
            break;
        default:                                        // no special memory channel active
            if (I2Cinput::getInstance()->isPressedMR()) // memory read switch
            {
                if (!memories.isWriteModeOn()) // write mode is off
                {
                    if (I2Cinput::getInstance()->wasPressedM())
                    {
                        Piezo::getInstance()->beepOK();
                        memories.setWriteModeOn(true);
                    }
                }
                else // write mode is on
                {
                    if (I2Cinput::getInstance()->wasPressedM())
                    {
                        TrxStateVfo *lastVfo = I2Cinput::getInstance()->isPressedAB() ? &vfoB : &vfoA;
                        saveMemory(memories.getMemoryIndex(), lastVfo->toMemory());
                        memories.setWriteModeOn(false);
                    }
                    else if (I2Cinput::getInstance()->isPressedAB())
                    {
                        deleteMemory(memories.getMemoryIndex());
                        memories.setWriteModeOn(false);
                    }
                    else if (I2Cinput::getInstance()->isPressedPtt() || wasPressed("rotaryButton") && isPressed("rotaryButton")) // leave write mode withut saving
                    {
                        Piezo::getInstance()->beepError();
                        memories.setWriteModeOn(false);
                    }
                }
            }
            else // mr is not pressed
            {
                memories.setWriteModeOn(false);

                if (I2Cinput::getInstance()->wasPressedM())
                {
                    Piezo::getInstance()->beepError();
                }
            }

            if (wasPressed("rotaryButton") && isPressed("rotaryButton")) // scanner
            {
                if (I2Cinput::getInstance()->isPressedPtt() || (mode == ctcss))
                {
                    Piezo::getInstance()->beepError();
                }
                else if (scanner.isOn())
                {
                    Piezo::getInstance()->beepOK();
                    scanner.setOn(false);
                }
                else // scanner is off and scan is allowed
                {
                    scanner.setOn(true);
                    Piezo::getInstance()->beepOK();
                }
            }
            else if (scanner.isOn())
            {
                if (I2Cinput::getInstance()->isPressedPtt() || (mode == ctcss))
                {
                    scanner.setOn(false);
                    Piezo::getInstance()->beepOK();
                }
                else
                {
                    setTxAllowed(false, pttSm);

                    if (updown != 0)
                    {
                        scanner.setUp(updown > 0);
                    }

                    scanner.update(currentState);
                }
            }
            else if (!I2Cinput::getInstance()->isPressedPtt())
            {
                setTxAllowed(false, pttSm);
                if (updown != 0)
                {
                    currentState->up(updown);
                }
            }
            else // PTT pressed
            // The VFO wheel and the UP/DOWN buttons should work always in SSB and CW.
            {
                // TODO
            }
        }

        // update peripherals
        Display::getInstance()->update(*currentState, scanner);

        if (currentState->getCurrentFrequency() != 0) // no unused memory channel
        {
            ADF4351::getInstance()->write(currentState->getCurrentFrequency()); // pll
        }

        // bool txAllowed = currentState->isTxAllowed() && !scanner.isOn();
        bool txAllowed = true; // TODO fix this
        setTxAllowed(txAllowed, pttSm);
    }
}

void setTxAllowed(const bool allowed, const uint pttSm)
{
    if (allowed)
    {
        pio_sm_exec(PTT_PIO, pttSm, pio_encode_jmp(ptt_offset_enable));
    }
    else
    {
        pio_sm_exec(PTT_PIO, pttSm, pio_encode_jmp(ptt_offset_disable));
    }
}