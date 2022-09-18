#include "timer.hpp"

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

void InitializeLAPICTimer() {
    divide_config = 0b1011u;
    lvt_timer = (0b001u < 16) | 32;
}

void StartLAPICTimer() {
    initial_count = kCountMax;
}

uint32_t LAPITimerElapsed() {
    return kCountMax - current_count;
}

void StopLAPITimer() {
    initial_count = 0;
}
