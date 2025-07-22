add_library(rp2040-Si5351
        Si5351.cpp)

target_link_libraries(rp2040-Si5351
        hardware_i2c
        pico_stdlib
        )
        
target_include_directories (rp2040-Si5351 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
