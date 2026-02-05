#pragma once

#include "hardware/pio.h"

#include "TrxState.h"


void beepOK(pio_sm_config *c, uint sm);
void beepWriteOK(pio_sm_config *c, uint sm);
void beepError(pio_sm_config *c, uint sm);

class Piezo
{
public:
    static Piezo* getInstance();
    void beepOK();
    void beepWriteOK();
    void beepError();
private:
    Piezo();
    uint beepSm; // state machine
};

class Ctcss
{
public:
    static Ctcss* getInstance();
    void update(TrxState &trxState);

private:
    Ctcss();
    uint ctcssSm; // state machine
    void setOn(const float frequency);
    void setOff();

    const uint32_t sysClock;
    const uint cycles {180}; // 180 cycles per wave, see ctcss.pio

    float frequency = 0;
    bool on = false;
};