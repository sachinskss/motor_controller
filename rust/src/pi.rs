pub struct PIController {
    pub kp: f32,
    pub ki: f32,
    integral: f32,
    out_min: f32,
    out_max: f32,
    sample_time: f32,
}

impl PIController {
    pub fn new(kp: f32, ki: f32, out_min: f32, out_max: f32, sample_time: f32) -> Self {
        PIController {
            kp,
            ki,
            integral: 0.0,
            out_min,
            out_max,
            sample_time,
        }
    }

    pub fn update(&mut self, error: f32) -> f32 {
        let proportional = self.kp * error;
        
        // Integral term with anti-windup
        let integral_term = self.ki * self.integral;
        let mut output = proportional + integral_term;

        // Clamp output before updating integral (better anti-windup)
        let clamped_output = output.max(self.out_min).min(self.out_max);

        // Update integral only if output is not saturated
        if output == clamped_output {
            self.integral += error * self.sample_time;
        }

        clamped_output
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
        let mut pi = PIController::new(1.0, 10.0, -100.0, 100.0, 0.01);
        let mut measurement = 0.0;
        let setpoint = 10.0;
        for _ in 0..1000 {
            let error = setpoint - measurement;
            let control = pi.update(error);
            measurement += control * 0.01; // simple plant
        }
        assert!((measurement - setpoint).abs() < 0.1, "Value was {}", measurement);
    }
}
