#include "timer.hpp"
#include "interrupt.hpp"

namespace {
    const uint32_t kCountMax = 0x000fffffu;
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
    divide_config = 0b1011u; // devide 1:1
    // 0-7   bit: 割り込みベクタ. 0x41 セット
    // 16    bit: 割り込みマスク. 0=割り込み許可 セット
    // 17-18 bit: タイマー動作モード. 1=周期 セット
    lvt_timer = (0b010u << 16) | InterruptVector::kLAPICTimer;
    initial_count = kCountMax;
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
