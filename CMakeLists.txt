cmake_minimum_required(VERSION 3.10)
project(MotorControllerSim LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build Rust library
set(RUST_MANIFEST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/rust/Cargo.toml)
add_custom_target(rust_build ALL
    COMMAND cargo build --manifest-path ${RUST_MANIFEST_PATH} --release
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building Rust library"
)

# Determine Rust static library path
if(WIN32)
    set(RUST_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/rust/target/release/motor_algo.lib)
else()
    set(RUST_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/rust/target/release/libmotor_algo.a)
endif()

# C++ executable
add_executable(sim
    cpp/src/main.cpp
    cpp/src/hal_gpio.cpp
    cpp/src/hal_timer.cpp
    cpp/src/hal_flash.cpp
    cpp/src/hal_spi.cpp
    cpp/src/bootloader.cpp
    cpp/src/motor_control.cpp
)

add_dependencies(sim rust_build)
target_include_directories(sim PRIVATE cpp/inc)
target_link_libraries(sim ${RUST_LIB_PATH})

# Windows: additional link libraries
if(WIN32)
    target_link_libraries(sim ws2_32 userenv)
endif()