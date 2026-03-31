# Mixed C++/Rust Motor Controller Simulation

This repository implements a complete motor controller simulation with Field Oriented Control (FOC), using **C++ for the simulation environment** and **Rust for the core control algorithms**. It is designed to facilitate embedded firmware development, algorithm prototyping, and cross-language FFI exploration.

## Key Features

- **Full Field Oriented Control (FOC)**: A complete FOC implementation in Rust, including Clarke, Park, and Inverse Park transforms, along with PI controllers for d-q axis current regulation.
- **Detailed Motor Model**: A Permanent Magnet Synchronous Motor (PMSM) model in Rust that simulates both electrical and mechanical dynamics.
- **Closed-Loop Control**: The C++ simulation now operates in a closed loop, feeding simulated sensor data from the motor model back into the Rust control algorithms.
- **Functional Bootloader**: A simulated bootloader in C++ that demonstrates firmware update procedures, including flash erase and programming.
- **Real-time GUI Tuner**: A Python-based GUI that connects to the simulation via TCP, allowing for live tuning of PI gains and current targets.
- **Simulation Realism**: The simulation includes models for sensor noise to better approximate real-world conditions.
- **Comprehensive Unit Tests**: The Rust code is validated with a suite of unit tests (`cargo test`).

## Getting Started

For detailed instructions on prerequisites, building, running, and testing the project, please refer to the **[INSTRUCTIONS.md](INSTRUCTIONS.md)** file.

### Quick Start Scripts

For convenience, you can use the provided quick start scripts:

*   **Windows**: `run_sim.bat` (from the project root)
*   **Linux/macOS**: `run_sim.sh` (from the project root)

These scripts will automatically build the project, start the C++ simulation in the background, and then launch the Python GUI tuner.

### Manual Build & Run

1.  **Build the Project**:
    ```bash
    # Create the C header from Rust code
    cbindgen --config rust/cbindgen.toml --crate motor_algo --output cpp/inc/motor_algo.h --lang c rust
    
    # Build the C++ and Rust code
    mkdir build && cd build
    cmake ..
    cmake --build .
    ```

2.  **Run the Simulation** (from the `build` directory):
    ```bash
    ./sim
    ```

3.  **Launch the GUI Tuner** (from the `python_tools` directory in a new terminal):
    ```bash
    python gui_tuner.py
    ```

## Project Structure

A detailed breakdown of the project's file structure is available in [INSTRUCTIONS.md](INSTRUCTIONS.md).
