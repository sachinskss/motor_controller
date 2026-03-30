#pragma once
#include <cstdint>

// Bootloader address ranges (simulated)
#define BOOTLOADER_START  0x08000000
#define APP_START         0x08010000

void Bootloader_Init();
void Bootloader_Run();  // checks for firmware update request
bool Bootloader_UpdateFirmware(const uint8_t* image, uint32_t size);
void JumpToApplication();