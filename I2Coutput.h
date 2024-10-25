#pragma once

#include "settings.h"

class I2Coutput
{
public:
    static I2Coutput* getInstance();
    void set_DDS_CTRL(bool high);
    void pulse_DDS_CTRL();

private:
    I2Coutput();                              
    I2Coutput(const I2Coutput&);                   // disable copy constructor              
    I2Coutput & operator = (const I2Coutput &);    // disable operator =
};