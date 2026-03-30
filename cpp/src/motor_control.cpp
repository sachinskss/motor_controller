#include "motor_control.h"
#include <cstdlib>

// Declare Rust FFI functions (simplified)
extern "C" {
    void* pi_create(float kp, float ki, float dt);
    float pi_update(void* pi, float setpoint, float measurement);
    void pi_free(void* pi);
    float add_two_floats(float a, float b);
}

MotorController::MotorController() : pi_speed(nullptr), pi_current_d(nullptr), pi_current_q(nullptr), v_d(0), v_q(0), speed_setpoint(0) {}

MotorController::~MotorController() {
    if (pi_speed) pi_free(pi_speed);
    if (pi_current_d) pi_free(pi_current_d);
    if (pi_current_q) pi_free(pi_current_q);
}

void MotorController::init(float kp_speed, float ki_speed, float kp_current, float ki_current) {
    const float dt = 0.0001f; // 100 us control period
    pi_speed = pi_create(kp_speed, ki_speed, dt);
    pi_current_d = pi_create(kp_current, ki_current, dt);
    pi_current_q = pi_create(kp_current, ki_current, dt);
}

void MotorController::setSpeedSetpoint(float rpm) {
    speed_setpoint = rpm;
}

void MotorController::update(float measured_speed, float measured_current_d, float measured_current_q) {
    // Speed loop outputs torque reference (converted to current reference)
    float torque_ref = pi_update(pi_speed, speed_setpoint, measured_speed);
    float i_q_ref = torque_ref; // simplified
    float i_d_ref = 0.0f; // MTPA not implemented
    
    // Current loops
    v_d = pi_update(pi_current_d, i_d_ref, measured_current_d);
    v_q = pi_update(pi_current_q, i_q_ref, measured_current_q);
}

float MotorController::getVoltageD() { return v_d; }
float MotorController::getVoltageQ() { return v_q; }