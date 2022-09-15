#pragma once

#include <stdint.h>

extern "C" {
    void IoOut32(uint16_t addr, uint32_t data);
    uint32_t IoIn32(uint16_t addr);
    uint64_t GetCS(void);
    void LoadIDT(uint64_t limit, uint64_t offset);
}
