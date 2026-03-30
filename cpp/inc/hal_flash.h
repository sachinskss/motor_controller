#pragma once
#include <cstdint>
#include <vector>

// Simulated flash memory
extern std::vector<uint8_t> flash_memory;
constexpr uint32_t FLASH_PAGE_SIZE = 1024;
constexpr uint32_t FLASH_SIZE = 64 * 1024; // 64 kB

void Flash_Init();
bool Flash_ErasePage(uint32_t address);
bool Flash_Write(uint32_t address, const uint8_t* data, uint32_t len);
bool Flash_Read(uint32_t address, uint8_t* buffer, uint32_t len);