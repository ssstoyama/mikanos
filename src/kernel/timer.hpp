#pragma once

#include <cstdint>
#include <queue>

#include "message.hpp"

class Timer {
public:
    Timer(unsigned long timeout, int value);
    unsigned long Timeout() const;
    int Value() const;

private:
    unsigned long timeout_;
    int value_;
};

inline bool operator <(const Timer& lhs, const Timer& rhs) {
    return lhs.Timeout() > rhs.Timeout();
}

class TimerManager {
public:
    TimerManager(std::deque<Message>& msg_queue);
    void AddTimer(const Timer& timer);
    void Tick();
    unsigned long CurrentTick() const;

private:
    volatile unsigned long tick_{0};
    std::priority_queue<Timer> timers_{};
    std::deque<Message>& msg_queue_;
};

const int kTimerFreq = 100;

extern TimerManager* timer_manager;
extern unsigned long lapic_timer_freq;

void InitializeLAPICTimer(std::deque<Message>& msg_queue);
void StartLAPICTimer();
uint32_t LAPICTimerElapsed();
void StopLAPICTimer();

void LAPICTimerOnInterrupt();
