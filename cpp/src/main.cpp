#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "hal_timer.h"
#include "hal_flash.h"
#include "bootloader.h"
#include "motor_control.h"

// Global simulation objects
MotorController motorCtrl;
Timer controlTimer(10000); // 10 kHz control loop
std::ofstream logFile;

void controlLoopISR() {
    static float time = 0.0;
    static float measured_speed = 0.0;
    static float measured_current_d = 0.0, measured_current_q = 0.0;
    
    motorCtrl.update(measured_speed, measured_current_d, measured_current_q);
    float v_d = motorCtrl.getVoltageD();
    float v_q = motorCtrl.getVoltageQ();
    
    // Here we would call Rust motor model step (simplified: just log)
    logFile << time << "," << measured_speed << "," << v_d << "," << v_q << std::endl;
    
    time += 0.0001; // 100 us step
}

int main() {
    Flash_Init();
    Bootloader_Init();
    
    // Check if bootloader should run
    Bootloader_Run(); // might jump to app or stay
    
    logFile.open("motor_log.csv");
    logFile << "time,speed,vd,vq\n";
    
    motorCtrl.init(0.5, 10.0, 1.0, 100.0);
    motorCtrl.setSpeedSetpoint(1000.0); // 1000 RPM
    
    controlTimer.attachInterrupt(controlLoopISR);
    controlTimer.start();
    
    // Run simulation for 5 seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    controlTimer.stop();
    logFile.close();
    std::cout << "Simulation finished. Log saved to motor_log.csv" << std::endl;
    return 0;
}