#pragma once

#include "TrxStateSpecialMemory.h"

#include "pico/stdlib.h"

class TrxStateScanMax: public TrxStateSpecialMemory
{
private:
    int32_t offset = 0;    
public:
    uint32_t getRxFrequency() const;
    void up(int n); 
    void save();
    bool isNotSaved() const;
};

inline TrxStateScanMax trxStateScanMax;