#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <cstdint>
#include "motor_algo.h" // Include the generated Rust FFI header

class MotorController {
public:
    MotorController();
    void init(float rs, float ls, float lambda_m, uint32_t p, float j, float b, float dt,
              float kp_id, float ki_id, float kp_iq, float ki_iq,
              float out_min_id, float out_max_id, float out_min_iq, float out_max_iq,
              float current_noise_std_dev, float angle_noise_std_dev);
    
    void update(float target_id, float target_iq, float load_torque,
                float v_bus, float i_a_raw, float i_b_raw, float i_c_raw);

    void get_abc_from_dq(float id, float iq, float electrical_angle, float* ia, float* ib, float* ic);

    float get_vd() const { return v_d_; }
    float get_vq() const { return v_q_; }
    float get_id() const { return id_; }
    float get_iq() const { return iq_; }
    float get_electrical_angle() const { return electrical_angle_; }
    float get_mechanical_speed() const { return mechanical_speed_; }

private:
    float v_d_;
    float v_q_;
    float id_;
    float iq_;
    float electrical_angle_;
    float mechanical_speed_;

    float current_noise_std_dev_;
    float angle_noise_std_dev_;
};

#endif // MOTOR_CONTROL_H
