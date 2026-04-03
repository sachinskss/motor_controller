# Motor Controller Simulation: Instructions

This document provides comprehensive instructions for building, running, and interacting with the mixed C++/Rust motor controller simulation.

## 1. Project Structure

```
mixed-cpp-rust-embedded-sim/
├── skill.md                 # Manus skill documentation
├── skill.yaml               # Manus skill metadata
├── CMakeLists.txt           # Top-level build configuration for C++
├── rust/                    # Rust algorithms with C FFI
│   ├── Cargo.toml           # Rust package manifest
│   ├── cbindgen.toml        # Configuration for generating C headers from Rust
│   └── src/
│       ├── lib.rs           # FFI exports, main control loop integration
│       ├── pi.rs            # PI controller implementation
│       ├── foc.rs           # Clarke/Park transforms, SVPWM
│       └── motor_model.rs   # Simulated motor model
├── cpp/                     # C++ simulation firmware
│   ├── inc/                 # Headers
│   │   ├── hal_*.h          # Simulated hardware peripherals (GPIO, Timer, Flash, SPI)
│   │   ├── bootloader.h     # Bootloader interface
│   │   ├── motor_control.h  # Motor controller class interface
│   │   └── comm_server.h    # TCP communication server interface
│   └── src/
│       ├── main.cpp         # Simulation entry point, main loop
│       ├── hal_*.cpp        # Mock HAL implementations
│       ├── bootloader.cpp   # Bootloader logic
│       ├── motor_control.cpp # Motor controller class implementation
│       └── comm_server.cpp  # TCP communication server implementation
├── python_tools/            # Analysis and tuning scripts
│   ├── plot_logs.py         # Script to plot simulation logs
│   └── gui_tuner.py         # Real-time GUI tuner
└── docs/
    └── design.md            # Detailed design document (placeholder)
```

## 2. Prerequisites

Before you begin, ensure you have the following installed:

*   **Rust**: Install via [rustup.rs](https://rustup.rs/).
*   **C++ Compiler**: A C++17 compatible compiler (e.g., GCC, Clang, MSVC).
*   **CMake**: Version 3.10 or higher.
*   **Python 3**: With `tkinter`, `numpy`, `matplotlib`, and `json` libraries. You can install them using pip:
    ```bash
    pip install numpy matplotlib
    # tkinter is usually part of Python installation, but might need `sudo apt-get install python3-tk` on Linux
    ```
*   **JsonCpp Library (Optional)**: Required for real-time GUI tuning.
    *   **Windows**: Recommended to use `vcpkg` or download pre-built binaries.
    *   **Ubuntu/Debian**: `sudo apt-get install -y libjsoncpp-dev`
    *   **Note**: If not installed, the project will still build, but JSON parsing in `main.cpp` will be disabled.

## 3. Building the Project

Follow these steps to build both the Rust library and the C++ simulation:

1.  **Navigate to the project root directory**:
    ```bash
    cd C:\Users\PC\Desktop\motor_controller-main
    ```

2.  **Generate Rust C-compatible Header**: This step uses `cbindgen` to create `motor_algo.h` from your Rust code.
    ```bash
    # Ensure cargo is in your PATH
    # If cbindgen is not installed, run: cargo install cbindgen
    cbindgen --config rust/cbindgen.toml --crate motor_algo --output cpp/inc/motor_algo.h --lang c rust
    ```
    *Note: The generated header is now wrapped in `extern "C"` for C++ compatibility.*

3.  **Create a build directory and run CMake**:
    ```bash
    mkdir build
    cd build
    cmake ..
    ```

4.  **Build the project**:
    ```bash
    cmake --build .
    ```

    Upon successful compilation, you will find the executable (e.g., `sim.exe` on Windows, `sim` on Linux) in the `build` directory.

## 4. Running the Simulation

### Quick Start Scripts

For convenience, you can use the provided quick start scripts:

*   **Windows**: `run_sim.bat` (from the project root)
*   **Linux/macOS**: `run_sim.sh` (from the project root)

These scripts will automatically start the C++ simulation in the background and then launch the Python GUI tuner.

### Manual Start

1.  **Start the C++ Simulation**: Navigate to the `build` directory and run the executable.
    ```cmd
    cd C:\Users\PC\Desktop\motor_controller-main\build
    .\sim.exe
    ```
    On Linux/macOS:
    ```bash
    cd build
    ./sim
    ```
    The simulation will start, print status messages, and begin logging data to `motor_log.csv`. It will also open a TCP server on port `65432` to listen for GUI tuner connections.

2.  **Run the Python GUI Tuner**: While the simulation is running, open a **new terminal** and navigate to the `python_tools` directory.
    ```cmd
    cd C:\Users\PC\Desktop\motor_controller-main\python_tools
    python gui_tuner.py
    ```
    On Linux/macOS:
    ```bash
    cd python_tools
    python gui_tuner.py
    ```
    The GUI tuner application will launch. It should automatically connect to the simulation. You can now adjust PI gains and target currents in real-time and send them to the simulation.

## 5. Analyzing Simulation Logs

After the C++ simulation completes (or is terminated), a `motor_log.csv` file will be generated in the `build` directory. You can use `python_tools/plot_logs.py` to visualize the data.

1.  **Navigate to the `python_tools` directory**:
    ```bash
    cd C:\Users\PC\Desktop\motor_controller-main\python_tools
    ```

2.  **Run the plotting script**:
    ```bash
    python plot_logs.py ..\build\motor_log.csv
    ```
    This script will generate plots of the motor's d/q currents, speed, and electrical angle over time.

## 6. Testing

### Rust Unit Tests

To run the Rust unit tests for the FOC, PI controller, and motor model:

```bash
cd C:\Users\PC\Desktop\motor_controller-main\rust
cargo test
```

### C++ Unit Tests (Future Work)

C++ unit tests (e.g., using GoogleTest) can be added to the `cpp` directory. This would involve configuring CMake to build and run these tests.

## 7. Troubleshooting (Windows)

*   **Linker Errors (LNK2019)**: Ensure `motor_algo.h` is wrapped in `extern "C"`. The project has been updated to handle this automatically.
*   **Socket Errors**: `comm_server.cpp` has been updated to use `Winsock2`. Ensure you link against `ws2_32.lib` (handled in `CMakeLists.txt`).
*   **Missing json/json.h**: If you don't have `jsoncpp` installed, the build will proceed with JSON parsing disabled. To enable it, install `jsoncpp` and uncomment the relevant lines in `main.cpp` and `CMakeLists.txt`.
