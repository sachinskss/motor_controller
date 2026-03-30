#include "hal_spi.h"
#include <iostream>

void SPI_Init() {
    std::cout << "[SPI] Initialized" << std::endl;
}

void SPI_Transmit(uint8_t* data, uint32_t len) {
    std::cout << "[SPI] Transmit " << len << " bytes" << std::endl;
}

void SPI_Receive(uint8_t* buffer, uint32_t len) {
    std::cout << "[SPI] Receive " << len << " bytes (simulated zeros)" << std::endl;
    for (uint32_t i = 0; i < len; ++i) buffer[i] = 0;
}

void SPI_TransmitReceive(uint8_t* tx, uint8_t* rx, uint32_t len) {
    SPI_Transmit(tx, len);
    SPI_Receive(rx, len);
}