https://github.com/michaelgroni/rp2040-Si5351

The class is not quite finished yet. At the moment it supports most of the features of the Si5351A chip. Nevertheless, it should also be sufficient for many applications with the other variants.

Contributions are welcome, preferably as commit requests.

# Example
## In CMakeLists.txt
```c++
add_library(Si5351 Si5351.cpp)
target_link_libraries(Si5351 pico_stdlib hardware_i2c)
```
## Main
```c++
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "Si5351.hpp"


const auto I2C_PORT {i2c0};
const uint8_t I2C_SDA {0};
const uint8_t I2C_SCL {1};


int main()
{
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    Si5351 si5351;
    si5351.setClkControl(0, false, false, 0, false, 3, 8);
    si5351.setPllInputSource(1);
    si5351.setPllParameters('a', 35, 0, 1033);
    si5351.setMultisynth0to5parameters(0, 90, 0, 15);
    si5351.resetPll();
    si5351.setOutput(0, true);

    return 0;
}
```

# Documentation
There is an [API documentation](https://michaelgroni.github.io/rp2040-Si5351). It is strongly recommended to read the Si5351 datasheet as well as Application Note 219.

# To do
- Throw exceptions instead of using default values or ignoring values.
- Fully support chip variants B and C.
- Add Multisynth6 and Multisynth7.
- Support spread spectrum.
- Support phase offset.
- Enable fanout.
- Support DIVBY4.
- Support multisynth integer mode.
