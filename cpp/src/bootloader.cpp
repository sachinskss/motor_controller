#include "bootloader.h"
#include "hal_flash.h"
#include <iostream>
#include <vector>
#include <cstring>

// Simulate a 64KB flash region for the application
static std::vector<uint8_t> simulated_app_flash(1024 * 64, 0xFF);

void Bootloader_Init() {
    std::cout << "[Bootloader] Initialized." << std::endl;
}

void Bootloader_Run() {
    // In a real system, this would check a GPIO pin, a UART command, or a flag in memory.
    // For this simulation, we can toggle this flag to test the update flow.
    bool update_requested = false;

    std::cout << "[Bootloader] Checking for firmware update request..." << std::endl;

    if (update_requested) {
        std::cout << "[Bootloader] Update request detected. Entering update mode." << std::endl;
        
        // Create a dummy firmware image for the simulation
        std::vector<uint8_t> new_firmware(1024, 0xAB); // 1KB of 0xAB
        strcpy((char*)new_firmware.data(), "NEW_FIRMWARE_V2");

        if (Bootloader_UpdateFirmware(new_firmware.data(), new_firmware.size())) {
            std::cout << "[Bootloader] Update successful. Rebooting..." << std::endl;
            // In a real system, this would trigger a system reset.
        } else {
            std::cerr << "[Bootloader] Update failed. Halting." << std::endl;
            return; // Halt
        }
    } else {
        std::cout << "[Bootloader] No update request. Proceeding to application." << std::endl;
    }
    
    JumpToApplication();
}

bool Bootloader_UpdateFirmware(const uint8_t* image, uint32_t size) {
    if (size > simulated_app_flash.size()) {
        std::cerr << "[Bootloader] Error: Firmware image is too large for flash!" << std::endl;
        return false;
    }

    std::cout << "[Bootloader] Starting firmware update of " << size << " bytes." << std::endl;

    HAL_FLASH_Unlock();

    // 1. Erase the required flash sectors
    std::cout << "[Bootloader] Erasing application flash sectors..." << std::endl;
    // Assuming a simple sector scheme for simulation
    HAL_FLASH_EraseSector(1);
    std::fill(simulated_app_flash.begin(), simulated_app_flash.end(), 0xFF);

    // 2. Program the new firmware image
    std::cout << "[Bootloader] Programming new firmware..." << std::endl;
    for (uint32_t i = 0; i < size; i += 4) {
        uint32_t data_chunk = 0;
        // Ensure we don't read past the end of the image
        uint32_t bytes_to_copy = (i + 4 <= size) ? 4 : (size - i);
        std::memcpy(&data_chunk, &image[i], bytes_to_copy);

        HAL_FLASH_Program(APP_START_ADDR + i, data_chunk);
        // Also write to our simulated flash vector
        std::memcpy(&simulated_app_flash[i], &data_chunk, bytes_to_copy);
    }

    HAL_FLASH_Lock();

    // 3. Verify the written data (optional but good practice)
    if (std::memcmp(simulated_app_flash.data(), image, size) != 0) {
        std::cerr << "[Bootloader] Error: Flash verification failed!" << std::endl;
        return false;
    }

    std::cout << "[Bootloader] Firmware update successful and verified." << std::endl;
    return true;
}

void JumpToApplication() {
    std::cout << "[Bootloader] Jumping to application at 0x" << std::hex << APP_START_ADDR << std::dec << "." << std::endl;
    // In a real embedded system, this would involve setting the Main Stack Pointer (MSP)
    // and jumping to the application's Reset_Handler address.
    // In this simulation, returning from this function allows main() to continue, which acts as our "application".
}
