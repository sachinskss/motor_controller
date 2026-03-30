# Mixed C++/Rust Embedded Simulation Skill

## Overview

This skill enables **mixed C++ and Rust development** for embedded systems with a focus on **motor control algorithms** and **hardware simulation**. It provides a complete framework for simulating a motor controller with Field Oriented Control (FOC), a hardware abstraction layer (HAL), a bootloader, and Python tooling—all without requiring physical hardware.

The skill is designed to assist developers, embedded engineers, and AI agents in:
- Prototyping embedded firmware
- Testing algorithms in a simulated environment
- Understanding FFI (Foreign Function Interface) between C++ and Rust
- Porting simulations to real microcontrollers (e.g., STM32)
- Generating and analyzing motor control logs

## Features

- **Dual‑language core**: Rust implements control algorithms (PI controllers, FOC transforms, motor model) with a C API; C++ simulates hardware peripherals and the application.
- **Simulated HAL**: GPIO, Timer, SPI, Flash – all mocked to behave like real MCU peripherals.
- **Bootloader simulation**: Firmware updates over UART (simulated) with flash management.
- **Motor control algorithms**:
  - PI controller with anti‑windup
  - Clarke/Park transforms
  - Space Vector PWM (placeholder for expansion)
  - Simple PMSM electrical/mechanical model
- **Python tooling**:
  - Plot logs (`matplotlib`)
  - GUI tuner (`tkinter`) for real‑time parameter adjustment (via simulated serial)
- **Cross‑platform build**: CMake builds the C++ executable and links the Rust static library.
- **Unit testing**: Rust `cargo test` verifies algorithms; C++ tests can be added (GoogleTest recommended).

## Getting Started

### Prerequisites

- **Rust** (>= 1.70): [Install via rustup](https://rustup.rs/)
- **C++ compiler** supporting C++17 (MSVC, MinGW, Clang, GCC)
- **CMake** (>= 3.10)
- **Python 3** (optional, for tooling) with `numpy`, `matplotlib`, `tkinter` (or `pandas` if desired)

### Installation

Clone the repository and build:

```bash
git clone <your-repo-url>
cd mixed-cpp-rust-embedded-sim
mkdir build && cd build
cmake ..
cmake --build .