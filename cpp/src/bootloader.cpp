#include "bootloader.h"
#include "hal_flash.h"
#include <iostream>
#include <cstring>

static bool firmware_update_requested = false;

void Bootloader_Init() {
    // Simulate reading a flag from flash
    uint8_t flag;
    Flash_Read(APP_START - 4, &flag, 1);
    firmware_update_requested = (flag == 0xAA);
}

void Bootloader_Run() {
    if (firmware_update_requested) {
        std::cout << "[Bootloader] Firmware update requested. Simulating update...\n";
        // Simulate receiving firmware image (e.g., from UART)
        uint8_t fake_image[256] = {0x00}; // dummy image
        Bootloader_UpdateFirmware(fake_image, sizeof(fake_image));
        // Clear flag
        uint8_t clear = 0x00;
        Flash_Write(APP_START - 4, &clear, 1);
        std::cout << "[Bootloader] Update complete. Jumping to application.\n";
    }
    JumpToApplication();
}

bool Bootloader_UpdateFirmware(const uint8_t* image, uint32_t size) {
    // Erase application pages
    for (uint32_t addr = APP_START; addr < APP_START + size; addr += FLASH_PAGE_SIZE) {
        Flash_ErasePage(addr);
    }
    // Write image
    return Flash_Write(APP_START, image, size);
}

void JumpToApplication() {
    std::cout << "[Bootloader] Jumping to application at 0x" << std::hex << APP_START << std::dec << std::endl;
    // In simulation, we just return (main will continue)
}