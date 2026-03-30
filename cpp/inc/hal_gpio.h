#pragma once
#include <cstdint>

enum class PinMode { INPUT, OUTPUT, ALTERNATE };
enum class PinState { LOW, HIGH };

void GPIO_Init(uint16_t pin, PinMode mode);
void GPIO_WritePin(uint16_t pin, PinState state);
PinState GPIO_ReadPin(uint16_t pin);
void GPIO_TogglePin(uint16_t pin);