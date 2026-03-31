#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <cstdint>

// Simulated memory layout
constexpr uint32_t BOOTLOADER_START_ADDR = 0x08000000;
constexpr uint32_t APP_START_ADDR = 0x08010000;

/**
 * @brief Initializes the bootloader.
 */
void Bootloader_Init();

/**
 * @brief Checks for an update request and either jumps to the app or enters update mode.
 */
void Bootloader_Run();

/**
 * @brief Simulates receiving and writing a new firmware image to flash.
 * @param image Pointer to the firmware image data.
 * @param size Size of the image in bytes.
 * @return True if the update was successful, false otherwise.
 */
bool Bootloader_UpdateFirmware(const uint8_t* image, uint32_t size);

/**
 * @brief Jumps to the main application.
 */
void JumpToApplication();

#endif // BOOTLOADER_H
