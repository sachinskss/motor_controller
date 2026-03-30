#include "hal_flash.h"
#include <cstring>
#include <iostream>

std::vector<uint8_t> flash_memory(FLASH_SIZE, 0xFF);

void Flash_Init() {
    std::cout << "[Flash] Initialized " << FLASH_SIZE << " bytes" << std::endl;
}

bool Flash_ErasePage(uint32_t address) {
    if (address >= FLASH_SIZE) return false;
    uint32_t page_start = (address / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE;
    for (uint32_t i = 0; i < FLASH_PAGE_SIZE && page_start + i < FLASH_SIZE; ++i) {
        flash_memory[page_start + i] = 0xFF;
    }
    std::cout << "[Flash] Erased page at 0x" << std::hex << page_start << std::dec << std::endl;
    return true;
}

bool Flash_Write(uint32_t address, const uint8_t* data, uint32_t len) {
    if (address + len > FLASH_SIZE) return false;
    std::memcpy(&flash_memory[address], data, len);
    std::cout << "[Flash] Wrote " << len << " bytes at 0x" << std::hex << address << std::dec << std::endl;
    return true;
}

bool Flash_Read(uint32_t address, uint8_t* buffer, uint32_t len) {
    if (address + len > FLASH_SIZE) return false;
    std::memcpy(buffer, &flash_memory[address], len);
    return true;
}