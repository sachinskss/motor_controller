#!/bin/bash

# Navigate to the build directory and run the simulation
cd build
./sim &
SIM_PID=$!
cd ..

echo "C++ Simulation started with PID: $SIM_PID"
echo "Waiting for simulation to initialize..."
sleep 2 # Give the C++ simulation a moment to start its TCP server

# Navigate to the python_tools directory and run the GUI tuner
cd python_tools
python gui_tuner.py &
GUI_PID=$!
cd ..

echo "Python GUI Tuner started with PID: $GUI_PID"

echo "To stop the simulation and GUI, run: kill $SIM_PID $GUI_PID"

wait $SIM_PID
wait $GUI_PID
