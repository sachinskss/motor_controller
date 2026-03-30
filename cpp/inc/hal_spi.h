#pragma once
#include <cstdint>

void SPI_Init();
void SPI_Transmit(uint8_t* data, uint32_t len);
void SPI_Receive(uint8_t* buffer, uint32_t len);
void SPI_TransmitReceive(uint8_t* tx, uint8_t* rx, uint32_t len);