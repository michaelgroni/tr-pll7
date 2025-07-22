#pragma once

#include "hardware/i2c.h"

#include <array>

class Si5351
{
private:
    i2c_inst_t* I2C_PORT;
    const uint8_t I2C_ADDR;

    /**
     * @brief Disables spread sprectrum.
     */
    void disableSpreadSpectrum() const;

    /**
     * @brief Returns a fractional pll or multisynth divider prepared for the Si5351.
     * @param a is the integer part.
     * @param b is the numerator.
     * @param c is the denominator.
     */
    std::array<uint32_t, 3> dividerParameters(const uint a, const uint b, const uint c) const; 
    
    /**
     * @brief Enables fanout.
     */
    void enableFanout();

    /**
     * @brief Reads a single byte from the Si5351 blocking.
     * @param reg ist the register to read from.
     * 
     */
    uint8_t readByte(uint8_t reg) const;
    
     /**
     * @brief Returns register contents calculated from divider parameters.
     * @param address is the adress of the first register to be written.
     * @param p is the return value of dividerParameters(...).
     * @return An array with the address of the first register, for example 0x42, followed by the calculated content.
     */   
    std::array<uint8_t, 9> registerContent(const uint8_t address, const std::array<uint32_t, 3> &p) const;

    /**
     * @brief Convenience method that calls `registerContent(address, p)` and `dividerParameters(a, b, c)`
     * `registerContent(address, a, b, c)` ist the same as `registerContent(address, dividerParameters(a, b, c))`
     * @param address is the adress of the first register to be written.
     * @param a is the integer part.
     * @param b is the numerator.
     * @param c is the denominator.
     * @return An array with the address of the first register, for example 0x42, followed by the calculated content.
     */
    std::array<uint8_t, 9> registerContent(const uint8_t address, const uint a, const uint b, const uint c) const;
    
    /**
     * @brief Sets the crystal internal load capacitance.
     * @param cLoad is the capacitance in pF and must bei 6, 8 or 10. Other values are treated as 10. 
     *
     */
    void setChrystalLoadCapacitance(uint8_t cLoad);
    
    /**
     * @brief Blocks until Si5351 is intialized and CLKIN is valid
     */
    void waitAfterPowerOn();

public:
    /**
     * @param i2cPort is an I2C controller, for example `i2c0`. It must have been initialized.
     * @param i2cAddr is the I2C address.
     * @param cLoad is the crystal cnternal load capacitance in pF and must bei 6, 8 or 10. Other values are treated as 10.
     */
    Si5351(i2c_inst* i2cPort = i2c0, uint8_t i2cAddr = 0x60, const uint8_t cLoad = 10);

    /**
     * @brief Disables the interrupt pin.
     */
    void disableInterrupts();

    /**
     * @brief Disables the OEB pin.
    */
    void disableOEBPin();

    /**
     * @brief Resets PLLA or PLLB.
     * @param pll must be 'a' or 'b'. Other values are ignored.
     */
    void resetPll(const char pll) const;

    /**
     * @brief Resets PLLA and PLLB.
     */
    void resetPll() const;

    /**
     * @param clkIndex must be between 0 and 7. Other values are treated like 0.
     * @param powerDown sets the mode of the output driver.
     * @param intMode sets the multisynth int mode.
     * @param inputSource must be 0 (PLLA) or 1 (PLLB on Si5351A/C or VCXO on Si5351B). Other values are treated like 0.
     * @param invert inverts the output clock if set to true.
     * @param outputSource must be 0 (XTAL) or 3 (multisynth). Other values are treated like 3.
     * @param strength must be 2, 4, 6 or 8. Other values are treated like 2.
     */
    void setClkControl(const uint8_t clkIndex, bool powerDown, bool intMode, uint8_t inputSource, bool invert, uint8_t outputSource, uint8_t strength);

    /**
     * @brief Configures the multisynth divider 0, 1, 2, 3, 4 or 5.
     * @param multisynth must be 0, 1, 2, 3, 4 or 5. Other values are ignored.
     * @param integer is a in (a + b/c).
     * @param num is b in (a + b/c).
     * @param denom is c in (a + b/c).
     * @param outDiv must not be greater than 7. Higher Bits are ignored. The output divider is set to 2^´outDiv´.
     */
    void setMultisynth0to5parameters(const uint8_t multisynth, const uint32_t integer, const uint32_t num, const uint32_t denom, uint8_t outDiv = 0) const;

    /**
     * @brief Determins the state of a disabled output.
     * @param clkIndex must be between 0 and 7. Other values are treated like 0.
     * @param disState must be 0 (low), 1 (high), 2 (high Z) or 3 (never disabled). Higher bits are ignored.
     */
    void setOutputDisableState(uint8_t clkIndex, const uint8_t disState);

    /**
     * @brief Enables or disables a single output driver.
     * @param clkIndex must bei 0, 1, 2, 3, 4, 5, 6 or 7. Other Values are ignored.
     * @param enabled enables output `clkIndex` if true is given, otherwise disables it.
     */
    void setOutput(const uint8_t clkIndex, const bool enabled);

    /**
     * @brief Disables all output drivers and powers them down.
     */
    void setOutputsOff();

    /**
     * @brief Sets the clock input divider.
     * @param inputDivider must be 1, 2, 4 or 8. Other values are treated like 1.
     * @param sourceB is ignored becauseat the moment only the Si5351A ist fully supported.
     * @param sourcea is ignored becauseat the moment only the Si5351A ist fully supported.
     */
    void setPllInputSource(const uint8_t inputDivider, const uint8_t sourceB=0, const uint8_t sourceA=0);

    /**
     * @brief Sets the PLL dividers.
     * @param pll must be 'a' (PLL A) or 'b' (PLL B). Other values are ignored.
     * @param integer is a in (a + b/c).
     * @param numerator is b in (a + b/c).
     * @param denominator is c in (a + b/c).
     */
    void setPllParameters(const char pll, const uint32_t integer, const uint32_t numerator, const uint32_t denominator);
};
