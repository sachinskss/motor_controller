#pragma once
#include <cstdint>

// C++ wrapper around Rust FFI functions
class MotorController {
public:
    MotorController();
    ~MotorController();
    void init(float kp_speed, float ki_speed, float kp_current, float ki_current);
    void setSpeedSetpoint(float rpm);
    void update(float measured_speed, float measured_current_d, float measured_current_q);
    float getVoltageD();
    float getVoltageQ();
private:
    void* pi_speed;    // opaque pointer to Rust PI controller
    void* pi_current_d;
    void* pi_current_q;
    float v_d, v_q;
    float speed_setpoint;
};