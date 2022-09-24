#pragma once

#include <cstdint>

class TimerManager {
public:
    void Tick();
    unsigned long CurrentTick() const;

private:
    volatile unsigned long tick_{0};
};

extern TimerManager* timer_manager;

void InitializeLAPICTimer();
void StartLAPICTimer();
uint32_t LAPITimerElapsed();
void StopLAPITimer();

void LAPICTimerOnInterrupt();
