#include "motor_control.h"
#include <iostream>
#include <random>

// Static random number generator for noise
static std::default_random_engine generator;
static std::normal_distribution<float> current_noise_distribution(0.0f, 1.0f);
static std::normal_distribution<float> angle_noise_distribution(0.0f, 1.0f);

MotorController::MotorController() : 
    v_d_(0.0f), 
    v_q_(0.0f), 
    id_(0.0f), 
    iq_(0.0f), 
    electrical_angle_(0.0f), 
    mechanical_speed_(0.0f),
    current_noise_std_dev_(0.0f),
    angle_noise_std_dev_(0.0f)
{
}

void MotorController::init(float rs, float ls, float lambda_m, uint32_t p, float j, float b, float dt,
                           float kp_id, float ki_id, float kp_iq, float ki_iq,
                           float out_min_id, float out_max_id, float out_min_iq, float out_max_iq,
                           float current_noise_std_dev, float angle_noise_std_dev) 
{
    // Call the Rust FFI function to initialize the motor model and PI controllers
    rust_init_motor_control(rs, ls, lambda_m, p, j, b, dt,
                            kp_id, ki_id, kp_iq, ki_iq,
                            out_min_id, out_max_id, out_min_iq, out_max_iq);

    current_noise_std_dev_ = current_noise_std_dev;
    angle_noise_std_dev_ = angle_noise_std_dev;

    std::cout << "MotorController initialized via Rust FFI." << std::endl;
}

void MotorController::update(float target_id, float target_iq, float load_torque,
                             float v_bus, float i_a_raw, float i_b_raw, float i_c_raw) 
{
    float i_a_noisy = i_a_raw + (current_noise_distribution(generator) * current_noise_std_dev_);
    float i_b_noisy = i_b_raw + (current_noise_distribution(generator) * current_noise_std_dev_);
    float i_c_noisy = i_c_raw + (current_noise_distribution(generator) * current_noise_std_dev_);

    float electrical_angle_noisy = electrical_angle_ + (angle_noise_distribution(generator) * angle_noise_std_dev_);

    // Call the main Rust FOC and motor model step function
    float ia_fb, ib_fb, ic_fb;
    rust_motor_step(target_id, target_iq, load_torque,
                    v_bus, i_a_noisy, i_b_noisy, i_c_noisy,
                    &v_d_, &v_q_, &id_, &iq_,
                    &electrical_angle_, &mechanical_speed_,
                    &ia_fb, &ib_fb, &ic_fb);
}

void MotorController::get_abc_from_dq(float id, float iq, float electrical_angle, float* ia, float* ib, float* ic) {
    rust_get_abc_from_dq(id, iq, electrical_angle, ia, ib, ic);
}
