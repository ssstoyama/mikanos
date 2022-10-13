#include "segment.hpp"

#include "asmfunc.h"
#include "interrupt.hpp"
#include "logger.hpp"
#include "memory_manager.hpp"

namespace {
    std::array<SegmentDescriptor, 7> gdt;
    std::array<uint32_t, 26> tss;

    static_assert((kTSS >> 3) + 1 < gdt.size());

    void setCodeSegment(
        SegmentDescriptor &desc,
        DescriptorType type,
        unsigned int descriptor_privilege_level,
        uint32_t base,
        uint32_t limit
    ) {
        desc.data = 0;

        // セグメントの開始アドレス
        desc.bits.base_low = base & 0xffffu;
        desc.bits.base_middle = (base >> 16) & 0xffu;
        desc.bits.base_high = (base >> 24) & 0xffu;
        // セグメントのバイト数
        desc.bits.limit_low = limit & 0xffffu;
        desc.bits.limit_high = (limit >> 16) & 0xfu;

        desc.bits.type = type;
        desc.bits.system_segment = 1; // code & data segment
        desc.bits.descriptor_privilege_level = descriptor_privilege_level;
        desc.bits.present = 1; // 有効な場合 1
        desc.bits.available = 0;
        desc.bits.long_mode = 1; // 1=64ビットモードのコードセグメント
        desc.bits.default_operation_size = 0; // long_mode が 1 の時は 0 をセット
        desc.bits.granualarity = 1; // 1=リミットを 4KiB 単位で解釈する
    }

    void setDataSegment(
        SegmentDescriptor &desc,
        DescriptorType type,
        unsigned int descriptor_privilege_type,
        uint32_t base,
        uint32_t limit
    ) {
        setCodeSegment(desc, type, descriptor_privilege_type, base, limit);

        desc.bits.long_mode = 0;
        desc.bits.default_operation_size = 1;
    }

    void setSystemSegment(SegmentDescriptor& desc,
                          DescriptorType type,
                          unsigned int descriptor_privilege_level,
                          uint32_t base,
                          uint32_t limit) {
        setCodeSegment(desc, type, descriptor_privilege_level, base, limit);
        desc.bits.system_segment = 0;
        desc.bits.long_mode = 0;
    }

    void setTSS(int index, uint64_t value) {
        tss[index]     = value & 0xffff'ffff;
        tss[index + 1] = value >> 32;
    }

    uint64_t allocateStackArea(int num_4kframes) {
        auto [stk, err] = memory_manager->Allocate(num_4kframes);
        if (err) {
            Log(kError, "failed to allocate stack area: %s\n", err.Name());
            exit(1);
        }
        return reinterpret_cast<uint64_t>(stk.Frame()) + num_4kframes * 4096;
    }
}

void SetupSegments() {
    gdt[0].data = 0; // unused
    setCodeSegment(gdt[1], DescriptorType::kExecuteRead, 0, 0, 0xfffff);
    setDataSegment(gdt[2], DescriptorType::kReadWrite, 0, 0, 0xfffff);
    setDataSegment(gdt[3], DescriptorType::kReadWrite, 3, 0, 0xfffff);
    setCodeSegment(gdt[4], DescriptorType::kExecuteRead, 3, 0, 0xfffff);
    LoadGDT(sizeof(gdt)-1, reinterpret_cast<uintptr_t>(&gdt[0]));
}

void InitializeSegmentation() {
    SetupSegments();
    SetDSAll(kKernelDS);
    SetCSSS(kKernelCS, kKernelSS);
}

void InitializeTSS() {
    setTSS(1, allocateStackArea(8));
    setTSS(7 + 2 * kISTForTimer, allocateStackArea(8));

    uint64_t tss_addr = reinterpret_cast<uint64_t>(&tss[0]);
    setSystemSegment(gdt[kTSS >> 3], DescriptorType::kTSSAvailable, 0,
                     tss_addr & 0xffff'ffff, sizeof(tss)-1);

    gdt[(kTSS >> 3) + 1].data = tss_addr >> 32;

    LoadTR(kTSS);
}
