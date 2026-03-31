@echo off

REM Navigate to the build directory and run the simulation
cd build
start /B sim.exe
set SIM_PID=!PID!
cd ..

echo C++ Simulation started with PID: %SIM_PID%
echo Waiting for simulation to initialize...
timeout /t 2 /nobreak > NUL

REM Navigate to the python_tools directory and run the GUI tuner
cd python_tools
start /B python gui_tuner.py
set GUI_PID=!PID!
cd ..

echo Python GUI Tuner started with PID: %GUI_PID%

echo To stop the simulation and GUI, use Task Manager or run: taskkill /PID %SIM_PID% /PID %GUI_PID%


pause
