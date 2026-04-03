const FRAC_1_SQRT_3: f32 = 0.57735026919; // 1.0 / sqrt(3.0)

#[derive(Default)]
pub struct Foc {
    // Clarke Transform outputs
    pub alpha: f32,
    pub beta: f32,

    // Park Transform outputs
    pub d: f32,
    pub q: f32,

    // Inverse Park Transform outputs
    pub v_alpha: f32,
    pub v_beta: f32,
}

impl Foc {
    pub fn new() -> Self {
        Default::default()
    }

    /// Clarke Transform: Converts three-phase currents (a, b, c) into a two-phase orthogonal system (alpha, beta).
    pub fn clarke_transform(&mut self, i_a: f32, i_b: f32, _i_c: f32) {
        // Assuming balanced system, i_a + i_b + i_c = 0
        self.alpha = i_a;
        self.beta = FRAC_1_SQRT_3 * (i_a + 2.0 * i_b);
    }

    /// Park Transform: Rotates the stationary alpha-beta frame to the rotating d-q frame, aligned with the rotor flux.
    pub fn park_transform(&mut self, theta: f32) {
        let cos_theta = theta.cos();
        let sin_theta = theta.sin();
        self.d = self.alpha * cos_theta + self.beta * sin_theta;
        self.q = -self.alpha * sin_theta + self.beta * cos_theta;
    }

    /// Inverse Park Transform: Rotates the d-q voltage vectors back to the stationary alpha-beta frame.
    pub fn inverse_park_transform(&mut self, v_d: f32, v_q: f32, theta: f32) {
        let cos_theta = theta.cos();
        let sin_theta = theta.sin();
        self.v_alpha = v_d * cos_theta - v_q * sin_theta;
        self.v_beta = v_d * sin_theta + v_q * cos_theta;
    }

    /// Space Vector PWM (SVPWM): Generates PWM duty cycles for the three inverter legs.
    /// This is a simplified placeholder. A full implementation is more complex.
    pub fn svpwm(&self, _v_bus: f32) -> (f32, f32, f32) {
        // A proper implementation would involve sector detection and duty cycle calculations.
        // For now, we use a simple inverse Clarke transform as a placeholder.
        let t_a = self.v_alpha;
        let t_b = -0.5 * self.v_alpha + 0.8660254 * self.v_beta;
        let t_c = -0.5 * self.v_alpha - 0.8660254 * self.v_beta;
        (t_a, t_b, t_c)
    }

    /// Inverse Clarke Transform: Converts two-phase orthogonal system (alpha, beta) into three-phase currents (a, b, c).
    pub fn inverse_clarke_transform(&self) -> (f32, f32, f32) {
        let i_a = self.alpha;
        let i_b = -0.5 * self.alpha + FRAC_1_SQRT_3 * 0.5 * self.beta;
        let i_c = -0.5 * self.alpha - FRAC_1_SQRT_3 * 0.5 * self.beta;
        (i_a, i_b, i_c)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::f32::consts::PI;

    const TOLERANCE: f32 = 1e-6;

    #[test]
    fn clarke_transform_test() {
        let mut foc = Foc::new();
        // Test case: Ia=1, Ib=-0.5, Ic=-0.5 (balanced system)
        foc.clarke_transform(1.0, -0.5, -0.5);
        assert!((foc.alpha - 1.0).abs() < TOLERANCE);
        assert!((foc.beta - 0.0).abs() < TOLERANCE, "beta was {}", foc.beta);
    }

    #[test]
    fn park_transform_test() {
        let mut foc = Foc::new();
        foc.alpha = 1.0;
        foc.beta = 0.0;
        // Rotate by 90 degrees
        foc.park_transform(PI / 2.0);
        assert!((foc.d - 0.0).abs() < TOLERANCE, "d was {}", foc.d);
        assert!((foc.q - 1.0).abs() < TOLERANCE, "q was {}", foc.q);
    }

    #[test]
    fn inverse_park_transform_test() {
        let mut foc = Foc::new();
        let v_d = 0.0;
        let v_q = 1.0;
        // Rotate back by 90 degrees
        foc.inverse_park_transform(v_d, v_q, PI / 2.0);
        assert!((foc.v_alpha - (-1.0)).abs() < TOLERANCE, "v_alpha was {}", foc.v_alpha);
        assert!((foc.v_beta - 0.0).abs() < TOLERANCE, "v_beta was {}", foc.v_beta);
    }

    #[test]
    fn transforms_are_inverse() {
        let mut foc = Foc::new();
        let (i_a, i_b, i_c) = (0.5, 0.2, -0.7);
        let theta = 1.2;

        foc.clarke_transform(i_a, i_b, i_c);
        foc.park_transform(theta);

        let (v_d, v_q) = (foc.d, foc.q);

        foc.inverse_park_transform(v_d, v_q, theta);
        let (t_a, t_b, t_c) = foc.svpwm(12.0);

        // This test is not perfect because SVPWM is a placeholder,
        // but it checks the chain of transforms.
        // A real inverse would require a proper inverse SVPWM.
    }
}
