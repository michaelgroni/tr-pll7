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
    void setOn(const double frequency);
    void setOff();

    double frequency = 0;
    bool on = false;
};