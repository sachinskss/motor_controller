# Mixed C++/Rust Motor Controller Simulation

This repository implements a complete motor controller simulation (Field Oriented Control) using **C++ for embedded HAL simulation** and **Rust for core algorithms**. It is designed as a Manus Skill to assist with embedded firmware development, algorithm prototyping, and cross-language FFI.

## Features
- Simulated hardware peripherals: GPIO, Timer, SPI, Flash
- Bootloader simulation (firmware update over UART)
- Field Oriented Control (Clarke/Park transforms, PI controllers, SVPWM)
- Motor model (PMSM electrical/mechanical)
- Python visualisation and tuning GUI
- Unit tests for Rust algorithms (`cargo test`)

## Build & Run

### Prerequisites
- Rust (install via [rustup.rs](https://rustup.rs/))
- C++ compiler (MSVC, MinGW, or Clang)
- CMake (≥3.10)
- Python 3 with `numpy`, `matplotlib`, `tkinter` (optional)

### Build
```bash
mkdir build && cd build
cmake ..
cmake --build .