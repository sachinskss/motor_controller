const FRAC_1_SQRT_3: f32 = 0.57735026919;
/// Represents a Permanent Magnet Synchronous Motor (PMSM) model.
use std::f32::consts::PI;
pub struct MotorModel {
    // Electrical parameters
    rs: f32,       // Stator resistance [Ohm]
    ls: f32,       // Stator inductance [H]
    lambda_m: f32, // Permanent magnet flux linkage [Wb]
    p: u32,        // Number of pole pairs

    // Mechanical parameters
    j: f32,        // Rotor inertia [kg*m^2]
    b: f32,        // Viscous friction coefficient [N*m*s/rad]

    // State variables
    pub i_d: f32,            // d-axis current [A]
    pub i_q: f32,            // q-axis current [A]
    pub electrical_angle: f32, // Electrical angle [rad]
    pub mechanical_angle: f32, // Mechanical angle [rad]
    pub electrical_speed: f32, // Electrical speed [rad/s]
    pub mechanical_speed: f32, // Mechanical speed [rad/s]

    // Simulation timestep
    dt: f32,
}

impl MotorModel {
    pub fn new(rs: f32, ls: f32, lambda_m: f32, p: u32, j: f32, b: f32, dt: f32) -> Self {
        MotorModel {
            rs,
            ls,
            lambda_m,
            p,
            j,
            b,
            i_d: 0.0,
            i_q: 0.0,
            electrical_angle: 0.0,
            mechanical_angle: 0.0,
            electrical_speed: 0.0,
            mechanical_speed: 0.0,
            dt,
        }
    }

    /// Performs one simulation step of the motor model.
    pub fn step(&mut self, v_d: f32, v_q: f32, load_torque: f32) {
        // Electrical model equations in the d-q frame
        // di_d/dt = (v_d - Rs*i_d + omega_e*Ls*i_q) / Ls
        // di_q/dt = (v_q - Rs*i_q - omega_e*Ls*i_d - omega_e*lambda_m) / Ls
        let omega_e = self.electrical_speed;

        let di_d_dt = (v_d - self.rs * self.i_d + omega_e * self.ls * self.i_q) / self.ls;
        let di_q_dt = (v_q - self.rs * self.i_q - omega_e * self.ls * self.i_d - omega_e * self.lambda_m) / self.ls;

        self.i_d += di_d_dt * self.dt;
        self.i_q += di_q_dt * self.dt;

        // Mechanical model equations
        // Te = 1.5 * P * (lambda_m * i_q + (Ld - Lq) * i_d * i_q)
        // For a surface PMSM, Ld = Lq, so the reluctance torque is zero.
        let electrical_torque = 1.5 * self.p as f32 * self.lambda_m * self.i_q;
        
        // d_omega_m/dt = (Te - B*omega_m - T_load) / J
        let d_mechanical_speed_dt = (electrical_torque - self.b * self.mechanical_speed - load_torque) / self.j;

        self.mechanical_speed += d_mechanical_speed_dt * self.dt;
        self.mechanical_angle += self.mechanical_speed * self.dt;

        // Update electrical speed and angle
        self.electrical_speed = self.mechanical_speed * self.p as f32;
        self.electrical_angle += self.electrical_speed * self.dt;

        // Normalize electrical angle to the range [0, 2*PI)
        self.electrical_angle = self.electrical_angle.rem_euclid(2.0 * PI);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    const TOLERANCE: f32 = 1e-3;

    fn get_default_model() -> MotorModel {
        MotorModel::new(0.1, 0.001, 0.05, 2, 0.0001, 0.00001, 0.0001)
    }

    #[test]
    fn motor_model_initialization() {
        let model = get_default_model();
        assert_eq!(model.i_d, 0.0);
        assert_eq!(model.i_q, 0.0);
        assert_eq!(model.electrical_angle, 0.0);
    }

    #[test]
    fn motor_model_step_no_load() {
        let mut model = get_default_model();
        // Apply some Vq to generate torque
        model.step(0.0, 1.0, 0.0);
        // Expect some q-axis current and speed to build up
        assert!(model.i_q > 0.0, "i_q should be positive");
        assert!(model.mechanical_speed > 0.0, "mechanical_speed should be positive");
    }

    #[test]
    fn motor_model_step_with_load() {
        let mut model = get_default_model();
        let load_torque = 0.001;
        // Apply Vq and a load torque
        model.step(0.0, 1.0, load_torque);
        
        let mut model_no_load = get_default_model();
        model_no_load.step(0.0, 1.0, 0.0);

        // Expect speed to be lower with load
        assert!(model.mechanical_speed < model_no_load.mechanical_speed);
    }
}
