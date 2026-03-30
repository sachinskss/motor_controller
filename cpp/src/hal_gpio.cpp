#include "hal_gpio.h"
#include <iostream>

void GPIO_Init(uint16_t pin, PinMode mode) {
    std::cout << "[GPIO] Init pin " << pin << " as " << (mode == PinMode::OUTPUT ? "OUTPUT" : "INPUT") << std::endl;
}

void GPIO_WritePin(uint16_t pin, PinState state) {
    std::cout << "[GPIO] Write pin " << pin << " = " << (state == PinState::HIGH ? "HIGH" : "LOW") << std::endl;
}

PinState GPIO_ReadPin(uint16_t pin) {
    // Simulate reading (always LOW for demo)
    return PinState::LOW;
}

void GPIO_TogglePin(uint16_t pin) {
    std::cout << "[GPIO] Toggle pin " << pin << std::endl;
}