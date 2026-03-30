#include "hal_timer.h"
#include <iostream>
#include <thread>
#include <chrono>

Timer::Timer(uint32_t frequency_hz) : period_us(1000000/frequency_hz), running(false) {}

void Timer::setPeriodUs(uint32_t us) { period_us = us; }
void Timer::start() { running = true; }
void Timer::stop() { running = false; }
void Timer::attachInterrupt(std::function<void()> cb) { callback = cb; }

void Timer::simulateElapsed() {
    if (running && callback) {
        callback();
    }
}