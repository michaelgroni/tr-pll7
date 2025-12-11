# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_CXX_ENABLE_RTTI 1)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(tr-pll70 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(tr-pll70 main.cpp )

pico_set_program_name(tr-pll70 "tr-pll70")
pico_set_program_version(tr-pll70 "0.3")

# Modify the below lines to enable/disable output over UART/USB
# pico_enable_stdio_uart(tr-pll70 0)
# pico_enable_stdio_usb(tr-pll70 0)

# Add the standard library to the build
target_link_libraries(tr-pll70
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(tr-pll70 PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)


add_subdirectory(pico-ssd1306) # OLED 

add_library(settings settings.cpp)
target_link_libraries(settings pico_stdlib hardware_i2c hardware_spi hardware_pio hardware_flash)

add_library(I2Cinput I2Cinput.cpp)
target_link_libraries(I2Cinput settings pico_stdlib hardware_i2c hardware_spi hardware_pio hardware_i2c hardware_timer)

add_library(TrxState TrxState.cpp)
target_link_libraries(TrxState pico_stdlib hardware_i2c settings hardware_spi hardware_pio I2Cinput)

add_library(TrxStateVfo TrxStateVfo.cpp)
target_link_libraries(TrxStateVfo TrxState memory pico_stdlib hardware_i2c hardware_spi hardware_pio)

add_library(GPIOoutput GPIOoutput.cpp)
pico_generate_pio_header(GPIOoutput ${CMAKE_CURRENT_LIST_DIR}/ctcss.pio) 
target_link_libraries(GPIOoutput settings TrxState pico_stdlib hardware_timer hardware_pio hardware_clocks)

add_library(memory memory.cpp)
target_link_libraries(memory settings TrxState GPIOoutput pico_stdlib hardware_flash hardware_sync)

add_library(GPIOinput GPIOinput.cpp)
target_link_libraries(GPIOinput settings pico_stdlib hardware_spi hardware_pio GPIOoutput)

add_library(ImproperFractionSi5351 ImproperFractionSi5351.cpp)
target_link_libraries(ImproperFractionSi5351 settings pico_stdlib)

add_library(ADF4351 ADF4351.cpp)
target_link_libraries(ADF4351 settings pico_stdlib hardware_spi hardware_pio ImproperFractionSi5351 )
add_subdirectory(rp2040-Si5351)
target_link_libraries(ADF4351 rp2040-Si5351 hardware_i2c)

add_library(TrxStateSpecialMemory TrxStateSpecialMemory.cpp)
target_link_libraries(TrxStateSpecialMemory TrxState)

add_library(TrxStateScanMin TrxStateScanMin.cpp)
target_link_libraries(TrxStateScanMin memory pico_stdlib TrxStateSpecialMemory settings)

add_library(TrxStateScanMax TrxStateScanMax.cpp)
target_link_libraries(TrxStateScanMax memory pico_stdlib TrxStateSpecialMemory settings)

add_library(TrxStateMemories TrxStateMemories.cpp)
target_link_libraries(TrxStateMemories pico_stdlib TrxState memory settings GPIOoutput GPIOinput)

add_library(setup setup.cpp)
target_link_libraries(setup pico_stdlib hardware_pio settings)

add_library(Display Display.cpp)
target_link_libraries(Display
        pico_stdlib 
        pico_ssd1306
        hardware_i2c 
        hardware_spi
        hardware_pio
        TrxStateVfo
        TrxStateSpecialMemory
        TrxStateScanMin
        TrxStateScanMax
        TrxStateMemories
        Scanner
        )

add_library(Scanner Scanner.cpp)
target_link_libraries(Scanner  
        pico_stdlib
        hardware_timer
        TrxState
        TrxStateVfo
        TrxStateScanMin
        TrxStateScanMax
        GPIOinput
        GPIOoutput
        I2Cinput
        TrxStateSpecialMemory
)

add_library(Fir Fir.cpp FirNotch.cpp FirLowpass.cpp)
target_link_libraries(Fir pico_stdlib settings)

add_library(Median Median.cpp)
target_link_libraries(Median pico_stdlib settings)

add_library(TrxStateSpecialMemoryFIR TrxStateSpecialMemoryFIR.cpp)
target_link_libraries(TrxStateSpecialMemoryFIR TrxStateSpecialMemory)

add_library(core1 core1.cpp)
pico_generate_pio_header(core1 ${CMAKE_CURRENT_LIST_DIR}/r2r.pio)
target_link_libraries(core1 pico_stdlib hardware_adc settings Fir Median)
 
pico_generate_pio_header(setup ${CMAKE_CURRENT_LIST_DIR}/rotaryEncoder.pio)
pico_generate_pio_header(GPIOoutput ${CMAKE_CURRENT_LIST_DIR}/beep.pio)
pico_generate_pio_header(setup ${CMAKE_CURRENT_LIST_DIR}/ptt.pio)


# Add any user requested libraries
target_link_libraries(tr-pll70
        pico_stdlib
        pico_multicore
        hardware_i2c
        hardware_spi 
        hardware_pio
        setup
        TrxState
        TrxStateVfo 
        Display
        GPIOinput
        GPIOoutput
        I2Cinput
        settings
        ADF4351
        Scanner
        TrxStateScanMin
        TrxStateScanMax
        TrxStateMemories
        TrxStateSpecialMemoryFIR
        memory
        Fir
        core1
        )


pico_add_extra_outputs(tr-pll70)

