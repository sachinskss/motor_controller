pub struct PI {
    kp: f32,
    ki: f32,
    dt: f32,
    integral: f32,
    limit_min: f32,
    limit_max: f32,
}

impl PI {
    pub fn new(kp: f32, ki: f32, dt: f32) -> Self {
        PI {
            kp,
            ki,
            dt,
            integral: 0.0,
            limit_min: -1e6,
            limit_max: 1e6,
        }
    }

    pub fn set_output_limits(&mut self, min: f32, max: f32) {
        self.limit_min = min;
        self.limit_max = max;
    }

    pub fn update(&mut self, setpoint: f32, measurement: f32) -> f32 {
        let error = setpoint - measurement;
        self.integral += error * self.dt;
        // Clamp integral
        self.integral = self.integral.clamp(self.limit_min, self.limit_max);
        let mut output = self.kp * error + self.ki * self.integral;
        output = output.clamp(self.limit_min, self.limit_max);
        output
    }

    pub fn reset(&mut self) {
        self.integral = 0.0;
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn pi_reaches_setpoint() {
        let mut pi = PI::new(1.0, 0.1, 0.01);
        let mut measurement = 0.0;
        let setpoint = 10.0;
        for _ in 0..1000 {
            let control = pi.update(setpoint, measurement);
            measurement += control * 0.01; // simple plant
        }
        assert!((measurement - setpoint).abs() < 0.1);
    }
}