#pragma once

#include "pico/stdlib.h"

#include "settings.h"

struct memory               // 16 bytes
{
    uint32_t rxFrequency;   // 4 bytes
    uint32_t txFrequency;   // 4 bytes
    uint8_t ctcssIndex;     // 1 byte
    bool isCtcssOn;         // 1 byte
    bool isUsed;            // 1 byte
    uint32_t padding1;      // 4 bytes
    uint8_t padding2;       // 1 byte
};

void flashInit();
uint32_t scanMin();
uint32_t scanMax();
void saveScanMin(uint32_t frequency);
void saveScanMax(uint32_t frequency);
void saveMemory(const size_t memoryIndex, const memory &m);
void deleteMemory(const size_t memoryIndex);

inline memory* flashData = (memory*) (XIP_BASE + MY_FLASH_DATA); // 256 * 16 bytes = 1 sector = 16 pages
/*
    flashData[0] contains fScanMin (rxFrequency) and fScanMax (txFrequeny).

    flashData[1] ... flashData[MEMORIES] are the memory channels.
*/