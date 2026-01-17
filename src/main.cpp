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




constexpr auto I2C_PORT_IO = i2c0;
constexpr auto I2C_PORT_SI5351A = i2c1;
constexpr uint8_t I2C_ADDR_SI5351A = 0x62; // 0x60, sometimes 0x62

void setTxAllowed(const bool allowed, const uint pttSm);

int main()
{

    // __asm volatile("bkpt 0");
    setupI2C(I2C_PORT_IO, I2C_PORT_SI5351A);
    
    setupGPIOinput();
    const auto pttSm = setupPTTpio();
    const auto rotarySm = setupRotaryPio();

    static I2Cinput i2cInput(I2C_PORT_IO);

    static TrxStateVfo vfoA(i2cInput, VFO_A_INIT);
    static TrxStateVfo vfoB(i2cInput, VFO_B_INIT);

    static TrxStateMemories memories(i2cInput);

    TrxStateScanMax trxStateScanMax(i2cInput);

    static Scanner scanner;

    setTxAllowed(false, pttSm);

    static TrxState *currentState = i2cInput.isPressedAB() ? &vfoB : &vfoA;

    static TrxStateSpecialMemoryFIR stateFir(i2cInput, currentState);
    TrxStateScanMin trxStateScanMin(i2cInput);

    flashInit();

    static auto firConfig = stateFir.getConfig();
    queue_init(&filterConfigQueue, sizeof(filterConfig), 2);
    queue_add_blocking(&filterConfigQueue, &firConfig);

    Piezo::getInstance()->beepOK();

    sleep_ms(100);
    ADF4351 adf4351(i2cInput, I2C_PORT_SI5351A, I2C_ADDR_SI5351A);  
    static Display display(I2C_PORT_IO);
    
    multicore_launch_core1(core1_entry);

    // main loop
    
    while (true)
    {
        sleep_ms(MAIN_LOOP_PAUSE_TIME);

        // read I²C input
        i2cInput.update(); // must be called in the main loop

        // select state instance
        switch (i2cInput.getSpecialMemoryChannel())
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
                currentState = &stateFir;
            }
            break;
        default: // no special memory channel active
            if (i2cInput.isPressedMR())
            {
                currentState = &memories;
            }
            else
            {
                currentState = i2cInput.isPressedAB() ? &vfoB : &vfoA;
            }
        }

        // read rotary encoder and up/down buttons
        int updown = readRotaryEncoder(rotarySm);
        // updown += readUpDownButtons(); // TODO implement up/down buttons on CONTROL IC1

        // read step buttons
        if (i2cInput.wasPressedStepIncrease())
        {
            currentState->stepUp();
            Piezo::getInstance()->beepOK();
        }
        if (i2cInput.wasPressedStepDecrease())
        {
            currentState->stepDown();
            Piezo::getInstance()->beepOK();
        }

        auto mode = i2cInput.getMode();

        switch (i2cInput.getSpecialMemoryChannel())
        {
        case 1: // special memory scan min
            if (updown != 0)
            {
                currentState->up(updown);
            }

            if (i2cInput.wasPressedM())
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

            if (i2cInput.wasPressedM())
            {
                Piezo::getInstance()->beepOK();
                trxStateScanMax.save();
            }
            break;
        case 3: // special memory filter
            if (wasPressed(GPIO_ROTARY_BUTTON) && isPressed(GPIO_ROTARY_BUTTON))
            {
                Piezo::getInstance()->beepError();
            }

            stateFir.up(updown);
            if (stateFir.wasChanged())
            {
                firConfig = stateFir.getConfig();
                queue_add_blocking(&filterConfigQueue, &firConfig);
            }
            break;
        default:                                        // no special memory channel active
            if (i2cInput.isPressedMR()) // memory read switch
            {
                if (!memories.isWriteModeOn()) // write mode is off
                {
                    if (i2cInput.wasPressedM())
                    {
                        Piezo::getInstance()->beepOK();
                        memories.setWriteModeOn(true);
                    }
                }
                else // write mode is on
                {
                    if (i2cInput.wasPressedM())
                    {
                        TrxStateVfo *lastVfo = i2cInput.isPressedAB() ? &vfoB : &vfoA;
                        saveMemory(memories.getMemoryIndex(), lastVfo->toMemory());
                        memories.setWriteModeOn(false);
                    }
                    else if (i2cInput.isPressedAB())
                    {
                        deleteMemory(memories.getMemoryIndex());
                        memories.setWriteModeOn(false);
                    }
                    else if (i2cInput.isPressedPtt() || wasPressed(GPIO_ROTARY_BUTTON) && isPressed(GPIO_ROTARY_BUTTON)) // leave write mode withut saving
                    {
                        Piezo::getInstance()->beepError();
                        memories.setWriteModeOn(false);
                    }
                }
            }
            else // mr is not pressed
            {
                memories.setWriteModeOn(false);

                if (i2cInput.wasPressedM())
                {
                    Piezo::getInstance()->beepError();
                }
            }

            if (wasPressed(GPIO_ROTARY_BUTTON) && isPressed(GPIO_ROTARY_BUTTON)) // scanner
            {
                if (i2cInput.isPressedPtt() || (mode == ctcss))
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
                if (i2cInput.isPressedPtt() || (mode == ctcss))
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
            else if (!i2cInput.isPressedPtt())
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
        display.update(*currentState, scanner);

        if (currentState->getCurrentFrequency() != 0) // no unused memory channel
        {
            adf4351.write(currentState->getCurrentFrequency()); // pll
        }

        // const bool txAllowed = currentState->isTxAllowed() && !scanner.isOn();
        const bool txAllowed = true; // TODO fix this
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