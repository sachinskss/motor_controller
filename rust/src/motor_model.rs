// Simple PMSM model (electrical + mechanical)
pub struct MotorModel {
    resistance: f32,    // stator resistance (ohms)
    inductance: f32,    // inductance (H)
    flux_linkage: f32,  // permanent magnet flux (Wb)
    inertia: f32,       // moment of inertia (kg*m^2)
    damping: f32,       // friction coefficient
    current_d: f32,
    current_q: f32,
    speed: f32,         // rad/s
    position: f32,      // rad
    dt: f32,
}

impl MotorModel {
    pub fn new(dt: f32) -> Self {
        MotorModel {
            resistance: 0.5,
            inductance: 0.001,
            flux_linkage: 0.1,
            inertia: 0.0001,
            damping: 0.00001,
            current_d: 0.0,
            current_q: 0.0,
            speed: 0.0,
            position: 0.0,
            dt,
        }
    }

    pub fn step(&mut self, v_d: f32, v_q: f32, load_torque: f32) {
        // Electrical equations (simplified)
        let di_d = (v_d - self.resistance * self.current_d + self.speed * self.inductance * self.current_q) / self.inductance;
        let di_q = (v_q - self.resistance * self.current_q - self.speed * self.inductance * self.current_d - self.speed * self.flux_linkage) / self.inductance;
        self.current_d += di_d * self.dt;
        self.current_q += di_q * self.dt;

        // Torque equation
        let torque = 1.5 * self.flux_linkage * self.current_q; // simplified
        // Mechanical equation
        let dw = (torque - load_torque - self.damping * self.speed) / self.inertia;
        self.speed += dw * self.dt;
        self.position += self.speed * self.dt;
    }

    pub fn get_currents(&self) -> (f32, f32, f32) {
        // DQ to ABC (simplified, just return d,q for now)
        (self.current_d, self.current_q, 0.0)
    }

    pub fn get_speed(&self) -> f32 { self.speed }
    pub fn get_position(&self) -> f32 { self.position }
}