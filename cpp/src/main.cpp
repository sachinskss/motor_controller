#include "motor_control.h"
#include "bootloader.h"
#include "comm_server.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <json/json.h>

// Global MotorController instance
MotorController motor_ctrl;

// Global variables for PI gains and targets, updated by GUI
float global_kp_id = 0.5f;
float global_ki_id = 10.0f;
float global_kp_iq = 0.5f;
float global_ki_iq = 10.0f;
float global_target_id = 0.0f;
float global_target_iq = 1.0f;

// Function to handle incoming data from the GUI tuner
void handle_gui_data(const std::string& data) {
    std::cout << "Received from GUI: " << data << std::endl;
    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(data, root);

    if (parsingSuccessful) {
        if (root.isMember("id_kp")) global_kp_id = root["id_kp"].asFloat();
        if (root.isMember("id_ki")) global_ki_id = root["id_ki"].asFloat();
        if (root.isMember("iq_kp")) global_kp_iq = root["iq_kp"].asFloat();
        if (root.isMember("iq_ki")) global_ki_iq = root["iq_ki"].asFloat();
        if (root.isMember("target_id")) global_target_id = root["target_id"].asFloat();
        if (root.isMember("target_iq")) global_target_iq = root["target_iq"].asFloat();

        // Re-initialize motor controller with new gains (simplified for now)
        // In a real system, PI controllers would have methods to update gains dynamically
        const float rs = 0.1f;       // Stator resistance [Ohm]
        const float ls = 0.0001f;    // Stator inductance [H]
        const float lambda_m = 0.01f; // PM flux linkage [Wb]
        const uint32_t p = 2;        // Pole pairs
        const float j = 0.00001f;    // Rotor inertia [kg*m^2]
        const float b = 0.000001f;   // Viscous friction [N*m*s/rad]
        const float dt = 0.0001f;    // Simulation timestep [s] (10 kHz)
        const float out_min = -12.0f, out_max = 12.0f; // Voltage limits
        const float current_noise_std_dev = 0.01f; // Example value
        const float angle_noise_std_dev = 0.001f; // Example value

        motor_ctrl.init(rs, ls, lambda_m, p, j, b, dt,
                        global_kp_id, global_ki_id, global_kp_iq, global_ki_iq,
                        out_min, out_max, out_min, out_max,
                        current_noise_std_dev, angle_noise_std_dev);

        std::cout << "Updated PI gains and targets from GUI.\n";
    } else {
        std::cerr << "Failed to parse JSON data from GUI: " << reader.getFormattedErrorMessages() << std::endl;
    }
}

int main() {
    std::cout << "Motor Controller Simulation Starting..." << std::endl;

    // --- Bootloader Initialization and Run ---
    Bootloader_Init();
    Bootloader_Run(); // This will either jump to app or handle update

    // If Bootloader_Run() returns, it means we are proceeding to the application (motor control simulation)

    // --- Initialize Communication Server ---
    CommServer gui_server(65432, handle_gui_data);
    gui_server.start();

    // --- Motor & Simulation Parameters ---
    const float rs = 0.1f;       // Stator resistance [Ohm]
    const float ls = 0.0001f;    // Stator inductance [H]
    const float lambda_m = 0.01f; // PM flux linkage [Wb]
    const uint32_t p = 2;        // Pole pairs
    const float j = 0.00001f;    // Rotor inertia [kg*m^2]
    const float b = 0.000001f;   // Viscous friction [N*m*s/rad]
    const float dt = 0.0001f;    // Simulation timestep [s] (10 kHz)

    // Initial PI controller gains and targets
    const float out_min = -12.0f, out_max = 12.0f; // Voltage limits
    const float current_noise_std_dev = 0.01f; // Example value
    const float angle_noise_std_dev = 0.001f; // Example value

    motor_ctrl.init(rs, ls, lambda_m, p, j, b, dt,
                    global_kp_id, global_ki_id, global_kp_iq, global_ki_iq,
                    out_min, out_max, out_min, out_max,
                    current_noise_std_dev, angle_noise_std_dev);

    // --- Simulation Loop ---
    float load_torque = 0.0f;   // Load torque [N*m]
    const float v_bus = 12.0f;    // DC bus voltage [V]

    // Initial current measurements (will be updated by motor model feedback)
    float i_a_feedback = 0.0f, i_b_feedback = 0.0f, i_c_feedback = 0.0f;

    std::ofstream log_file("motor_log.csv");
    log_file << "time,id,iq,speed_rpm,elec_angle,ia,ib,ic\n";

    const int num_steps = 20000; // Simulate for 2 seconds
    for (int i = 0; i < num_steps; ++i) {
        motor_ctrl.update(global_target_id, global_target_iq, load_torque,
                           v_bus, i_a_feedback, i_b_feedback, i_c_feedback);

        // Update feedback currents from the motor model for the next step
        // This is a simplified way to close the loop for simulation purposes.
        // In a real system, these would come from ADC readings.
        // For now, we need to convert motor_ctrl.id_ and motor_ctrl.iq_ back to a,b,c
        // This requires an inverse Park and inverse Clarke transform in C++ or Rust.
        // For simplicity in this simulation, we'll assume the motor model's internal
        // d/q currents are directly measurable for feedback after transformation.
        // A more robust solution would involve adding a function to Rust to convert
        // d/q currents to a/b/c based on the current electrical angle.

        // For now, we'll just use the internal d/q currents for feedback, which is not entirely accurate
        // but demonstrates the closed-loop concept.
        // TODO: Implement proper inverse Park and Clarke in Rust to get i_a, i_b, i_c from motor.i_d, motor.i_q
        // Close the loop: get the simulated phase currents from the motor model for the next step
        motor_ctrl.get_abc_from_dq(motor_ctrl.get_id(), motor_ctrl.get_iq(), motor_ctrl.get_electrical_angle(),
                                  &i_a_feedback, &i_b_feedback, &i_c_feedback);

        // Log data for analysis
        if (i % 10 == 0) { // Log every 10 steps (1ms)
            log_file << (i * dt) << ","
                     << motor_ctrl.get_id() << ","
                     << motor_ctrl.get_iq() << ","
                     << motor_ctrl.get_mechanical_speed() * 30.0 / 3.14159f << "," // rad/s to RPM
                     << motor_ctrl.get_electrical_angle() << ","
                     << i_a_feedback << "," << i_b_feedback << "," << i_c_feedback << "\n";
        }
    }

    log_file.close();
    gui_server.stop(); // Stop the server when simulation ends
    std::cout << "Simulation finished. Log saved to motor_log.csv" << std::endl;

    return 0;
}
