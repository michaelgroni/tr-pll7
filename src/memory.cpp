#include "memory.h"
#include "settings.h"
#include "GPIOoutput.h"

#include "hardware/flash.h"
#include "hardware/sync.h"

#include <cstring> // memcpy

uint8_t UNIQUE_ID;

void flashInit()
{
    // test if we already stored data in the flash memory
    flash_get_unique_id(&UNIQUE_ID);
    uint8_t *firstFlashByte = (uint8_t *)(XIP_BASE + MY_FLASH_OFFSET);

    if (UNIQUE_ID != *firstFlashByte) // no data in the flash memory, write for the first time
    {
        struct memory tempData[256];

        tempData[0].rxFrequency = F_MIN; // fScanMin
        tempData[0].txFrequency = F_MAX; // fScanMax

        for (size_t i = 1; i <= MEMORIES; i++)
        {
            tempData[i].isUsed = false;
        }

        auto interruptState = save_and_disable_interrupts();

        // write unique id
        flash_range_erase(MY_FLASH_OFFSET, FLASH_SECTOR_SIZE);             // erase 1 sector = 4096 bytes
        flash_range_program(MY_FLASH_OFFSET, &UNIQUE_ID, FLASH_PAGE_SIZE); // write 1 page = 256 bytes

        // write flash data
        flash_range_erase(MY_FLASH_DATA, FLASH_SECTOR_SIZE); // erase 1 sector = 4096 bytes
        flash_range_program(MY_FLASH_DATA, (uint8_t *)tempData, 4096);

        restore_interrupts(interruptState);
    }
}

uint32_t scanMin()
{
    return flashData[0].rxFrequency;
}

uint32_t scanMax()
{
    return flashData[0].txFrequency;
}

void saveScanMin(uint32_t frequency)
{
    Piezo::getInstance()->beepWriteOK();
    
    struct memory tempData[256];
    memcpy(tempData, flashData, 4096);

    tempData[0].rxFrequency = frequency;

    auto interruptState = save_and_disable_interrupts();
    flash_range_erase(MY_FLASH_DATA, FLASH_SECTOR_SIZE); // erase 1 sector = 4096 bytes
    flash_range_program(MY_FLASH_DATA, (uint8_t *)tempData, 4096);
    restore_interrupts(interruptState);
}

void saveScanMax(uint32_t frequency)
{
    Piezo::getInstance()->beepWriteOK();

    struct memory tempData[256];
    memcpy(tempData, flashData, 4096);

    tempData[0].txFrequency = frequency;

    auto interruptState = save_and_disable_interrupts();
    flash_range_erase(MY_FLASH_DATA, FLASH_SECTOR_SIZE); // erase 1 sector = 4096 bytes
    flash_range_program(MY_FLASH_DATA, (uint8_t *)tempData, 4096);
    restore_interrupts(interruptState);
}


void saveMemory(const size_t memoryIndex, const memory &m)
{
    if (memoryIndex < 1 || memoryIndex > MEMORIES)
    {
        Piezo::getInstance()->beepError();
    }
    else
    {
        Piezo::getInstance()->beepWriteOK();

        struct memory tempData[256];
        memcpy(tempData, flashData, 4096);

        tempData[memoryIndex] = m;

        auto interruptState = save_and_disable_interrupts();
        flash_range_erase(MY_FLASH_DATA, FLASH_SECTOR_SIZE); // erase 1 sector = 4096 bytes
        flash_range_program(MY_FLASH_DATA, (uint8_t *)tempData, 4096);
        restore_interrupts(interruptState);
    }
}


void deleteMemory(const size_t memoryIndex)
{
    if (memoryIndex < 1 || memoryIndex > MEMORIES)
    {
        Piezo::getInstance()->beepError();
    }
    else
    {
        Piezo::getInstance()->beepWriteOK();

        struct memory tempData[256];
        memcpy(tempData, flashData, 4096);

        tempData[memoryIndex].isUsed = false;

        auto interruptState = save_and_disable_interrupts();
        flash_range_erase(MY_FLASH_DATA, FLASH_SECTOR_SIZE); // erase 1 sector = 4096 bytes
        flash_range_program(MY_FLASH_DATA, (uint8_t *)tempData, 4096);
        restore_interrupts(interruptState);
    }
}