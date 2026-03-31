mod pi;
mod foc;
mod motor_model;

use pi::PIController;
use foc::Foc;
use motor_model::MotorModel;
const FRAC_1_SQRT_3: f32 = 0.57735026919;

// Define a static mutable MotorModel and PI controllers for FOC
static mut MOTOR_MODEL: Option<MotorModel> = None;
static mut ID_CONTROLLER: Option<PIController> = None;
static mut IQ_CONTROLLER: Option<PIController> = None;

#[no_mangle]
pub extern "C" fn rust_init_motor_control(rs: f32, ls: f32, lambda_m: f32, p: u32, j: f32, b: f32, dt: f32,
                                        kp_id: f32, ki_id: f32, kp_iq: f32, ki_iq: f32,
                                        out_min_id: f32, out_max_id: f32, out_min_iq: f32, out_max_iq: f32) {
    unsafe {
        MOTOR_MODEL = Some(MotorModel::new(rs, ls, lambda_m, p, j, b, dt));
        ID_CONTROLLER = Some(PIController::new(kp_id, ki_id, out_min_id, out_max_id, dt));
        IQ_CONTROLLER = Some(PIController::new(kp_iq, ki_iq, out_min_iq, out_max_iq, dt));
    }
}

#[no_mangle]
pub extern "C" fn rust_motor_step(target_id: f32, target_iq: f32, load_torque: f32,
                                  v_bus: f32, i_a: f32, i_b: f32, i_c: f32,
                                  output_vd: *mut f32, output_vq: *mut f32,
                                  output_id: *mut f32, output_iq: *mut f32,
                                  output_electrical_angle: *mut f32,
                                  output_mechanical_speed: *mut f32,
                                  output_ia_fb: *mut f32, output_ib_fb: *mut f32, output_ic_fb: *mut f32) {
    unsafe {
        let motor = MOTOR_MODEL.as_mut().expect("MotorModel not initialized");
        let id_ctrl = ID_CONTROLLER.as_mut().expect("ID Controller not initialized");
        let iq_ctrl = IQ_CONTROLLER.as_mut().expect("IQ Controller not initialized");

        // 1. Clarke Transform (using measured currents)
        let mut foc = Foc::new();
        foc.clarke_transform(i_a, i_b, i_c);

        // 2. Park Transform (using current electrical angle from motor model)
        foc.park_transform(motor.electrical_angle);

        // 3. Current PI Controllers
        let id_error = target_id - foc.d;
        let iq_error = target_iq - foc.q;

        let v_d_control = id_ctrl.update(id_error);
        let v_q_control = iq_ctrl.update(iq_error);

        // 4. Inverse Park Transform
        foc.inverse_park_transform(v_d_control, v_q_control, motor.electrical_angle);

        // 5. SVPWM (simplified)
        let (_t_a, _t_b, _t_c) = foc.svpwm(v_bus);

        // 6. Motor Model Step
        motor.step(v_d_control, v_q_control, load_torque);

        // Output results
        *output_vd = v_d_control;
        *output_vq = v_q_control;
        *output_id = motor.i_d;
        *output_iq = motor.i_q;
        *output_electrical_angle = motor.electrical_angle;
        *output_mechanical_speed = motor.mechanical_speed;

        // For feedback, convert motor model's d/q currents back to a/b/c
        let mut foc_feedback = Foc::new();
        foc_feedback.inverse_park_transform(motor.i_d, motor.i_q, motor.electrical_angle);
        let (ia_fb, ib_fb, ic_fb) = foc_feedback.inverse_clarke_transform();
        *output_ia_fb = ia_fb;
        *output_ib_fb = ib_fb;
        *output_ic_fb = ic_fb;
    }
}

#[no_mangle]
pub extern "C" fn rust_add(a: i32, b: i32) -> i32 {
    a + b
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_rust_add() {
        assert_eq!(rust_add(1, 2), 3);
    }

    #[test]
    fn test_motor_control_init_and_step() {
        // Initialize with some dummy parameters
        let rs = 0.1; let ls = 0.001; let lambda_m = 0.05; let p = 2; let j = 0.0001; let b = 0.00001; let dt = 0.0001;
        let kp_id = 0.5; let ki_id = 5.0; let kp_iq = 0.5; let ki_iq = 5.0;
        let out_min = -10.0; let out_max = 10.0;

        unsafe {
            rust_init_motor_control(rs, ls, lambda_m, p, j, b, dt,
                                    kp_id, ki_id, kp_iq, ki_iq,
                                    out_min, out_max, out_min, out_max);

            let mut vd = 0.0; let mut vq = 0.0;
            let mut id = 0.0; let mut iq = 0.0;
            let mut electrical_angle = 0.0; let mut mechanical_speed = 0.0;

            // Simulate a few steps with target iq and no load
            for _ in 0..100 {
                rust_motor_step(0.0, 1.0, 0.0, 12.0, 0.0, 0.0, 0.0, // dummy currents for now
                                &mut vd, &mut vq, &mut id, &mut iq,
                                &mut electrical_angle, &mut mechanical_speed);
            }

            // Basic assertions to check if values changed
            assert!(id.abs() < 0.1); // d-axis current should be controlled to near zero
            assert!(iq > 0.5); // q-axis current should be driven towards target
            assert!(mechanical_speed > 0.0);
            assert!(electrical_angle > 0.0);
        }
    }
}

#[no_mangle]
pub extern "C" fn rust_get_abc_from_dq(id: f32, iq: f32, electrical_angle: f32,
                                      output_ia: *mut f32, output_ib: *mut f32, output_ic: *mut f32) {
    unsafe {
        let mut foc = Foc::new();
        foc.inverse_park_transform(id, iq, electrical_angle);
        let (ia, ib, ic) = foc.inverse_clarke_transform();
        *output_ia = ia;
        *output_ib = ib;
        *output_ic = ic;
    }
}
