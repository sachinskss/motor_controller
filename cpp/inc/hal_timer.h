#pragma once
#include <cstdint>
#include <functional>

class Timer {
public:
    Timer(uint32_t frequency_hz);
    void setPeriodUs(uint32_t us);
    void start();
    void stop();
    void attachInterrupt(std::function<void()> callback);
    void simulateElapsed();  // called by main simulation loop
private:
    uint32_t period_us;
    bool running;
    std::function<void()> callback;
};