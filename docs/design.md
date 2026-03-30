# Design Document

## Architecture
- **Rust** implements low‑level algorithms (PI, FOC, motor model) and exposes a C API.
- **C++** simulates the MCU hardware peripherals (GPIO, Timer, SPI, Flash) and a bootloader.
- **Python** provides post‑simulation analysis and a tuning GUI.

## FFI Design
Rust functions are marked `#[no_mangle]` and `extern "C"` to be callable from C++. Opaque pointers (`*mut PI`) are used to maintain Rust’s ownership.

## Simulation Strategy
- Time is advanced by the main loop calling `Timer::simulateElapsed()` at a fixed step.
- The motor model runs in Rust (called from C++) to update currents and speed.
- All logs are written to CSV for Python plotting.

## Porting to Real Hardware
- Replace simulated HAL with vendor HAL (e.g., STM32Cube).
- Use DMA and timer interrupts for control loops.
- Adapt bootloader to use actual UART/CAN and flash drivers.