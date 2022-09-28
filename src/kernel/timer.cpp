#include "timer.hpp"
#include "interrupt.hpp"
#include "acpi.hpp"
#include "task.hpp"

TimerManager::TimerManager(std::deque<Message> &msg_queue):
    msg_queue_{msg_queue} {
    timers_.push(Timer{std::numeric_limits<unsigned long>::max(), -1});
}

void TimerManager::AddTimer(const Timer& timer) {
    timers_.push(timer);
}

bool TimerManager::Tick() {
    ++tick_;

    bool task_timer_timeout = false;
    while (true) {
        const auto &t = timers_.top();
        if (t.Timeout() > tick_) {
            break;
        }

        if (t.Value() == kTaskTimerValue) {
            task_timer_timeout = true;
            timers_.pop();
            timers_.push(Timer{tick_+kTaskTimerPeriod, kTaskTimerValue});
            continue;
        }

        Message msg{Message::kTimerTimeout};
        msg.arg.timer.timeout = t.Timeout();
        msg.arg.timer.value = t.Value();
        msg_queue_.push_back(msg);

        timers_.pop();
    }

    return task_timer_timeout;
}

unsigned long TimerManager::CurrentTick() const {
    return tick_;
}

Timer::Timer(unsigned long timeout, int value):
    timeout_{timeout}, value_{value} {}

unsigned long Timer::Timeout() const {
    return timeout_;
}

int Timer::Value() const { return value_; }

TimerManager* timer_manager;
unsigned long lapic_timer_freq;

namespace {
    const uint32_t kCountMax = 0xffffffffu;
    // 割り込みの発生方法の設定など
    volatile uint32_t &lvt_timer     = *reinterpret_cast<uint32_t *>(0xfee00320);
    // カウンタの初期値
    volatile uint32_t &initial_count = *reinterpret_cast<uint32_t *>(0xfee00380);
    // カウンタの現在地
    volatile uint32_t &current_count = *reinterpret_cast<uint32_t *>(0xfee00390);
    // カウンタの減少スピードの設定
    volatile uint32_t &divide_config = *reinterpret_cast<uint32_t *>(0xfee003e0);
}

void InitializeLAPICTimer(std::deque<Message>& msg_queue) {
    timer_manager = new TimerManager(msg_queue);

    divide_config = 0b1011u; // devide 1:1
    // 16    bit: 割り込みマスク. 1=割り込み不許可 セット
    lvt_timer = 0b001u << 16; // masked;

    StartLAPICTimer();
    acpi::WaitMilliseconds(100); // 0.1s 待機
    const auto elapsed = LAPICTimerElapsed(); // 0.1s 分のカウント計測
    StopLAPICTimer();

    lapic_timer_freq = static_cast<unsigned long>(elapsed) * 10; // 0.1s * 10 = 1s 分のカウントに変換

    divide_config = 0b1011u;
    // 0-7   bit: 割り込みベクタ. 0x41 セット
    // 16    bit: 割り込みマスク. 0=割り込み許可 セット
    // 17-18 bit: タイマー動作モード. 1=周期 セット
    lvt_timer = (0b010u << 16) | InterruptVector::kLAPICTimer;
    // 1 / kTimerFreq 秒ごとに割り込みが発生する
    initial_count = lapic_timer_freq / kTimerFreq;
}

void StartLAPICTimer() {
    initial_count = kCountMax;
}

uint32_t LAPICTimerElapsed() {
    return kCountMax - current_count;
}

void StopLAPICTimer() {
    initial_count = 0;
}

void LAPICTimerOnInterrupt() {
    const bool task_timer_timeout = timer_manager->Tick();
    NotifyEndOfInterrupt();

    if (task_timer_timeout) {
        SwitchTask();
    }
}
