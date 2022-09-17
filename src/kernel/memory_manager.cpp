#include "memory_manager.hpp"
#include "logger.hpp"

BitmapMemoryManager::BitmapMemoryManager()
    : alloc_map_{}, range_begin_{FrameID{0}}, range_end_{FrameID{kFrameCount}} {}

WithError<FrameID> BitmapMemoryManager::Allocate(size_t num_frames) {
    size_t start_frame_id = range_begin_.ID();
    while (true) {
        size_t i = 0;
        for (; i < num_frames; ++i) {
            if (start_frame_id+i >= range_end_.ID()) {
                return {kNullFrame, MAKE_ERROR(Error::kNoEnoughMemory)};
            }
            if (getBit(FrameID{start_frame_id+i})) {
                // start_frame_id+i にあるフレームは割り当て済み
                break;
            }
        }
        if (i == num_frames) {
            MarkAllocated(FrameID{start_frame_id}, num_frames);
            return {
                FrameID{start_frame_id},
                MAKE_ERROR(Error::kSuccess)
            };
        }

        start_frame_id += i + 1;
    }
}

Error BitmapMemoryManager::Free(FrameID start_frame, size_t num_frames) {
    for (size_t i = 0; i < num_frames; ++i) {
        setBit(FrameID{start_frame.ID()+i}, false);
    }

    return MAKE_ERROR(Error::kSuccess);
}

void BitmapMemoryManager::MarkAllocated(FrameID start_frame, size_t num_frames) {
    for (size_t i = 0; i < num_frames; ++i) {
        setBit(FrameID{start_frame.ID()+i}, true);
    }
}

void BitmapMemoryManager::SetMemoryRange(FrameID range_begin, FrameID range_end) {
    range_begin_ = range_begin;
    range_end_ = range_end;
    Log(kDebug, "SetMemoryRange: begin=%d, end=%d\n", range_begin_.ID(), range_end_.ID());
}

bool BitmapMemoryManager::getBit(FrameID frame) const {
    auto line_index = frame.ID() / kBitsPerMapLine;
    auto bit_index = frame.ID() % kBitsPerMapLine;

    Log(kDebug, "getBit: frame.ID=%d, line_index=%d, bit_index=%d\n", frame.ID(), line_index, bit_index);

    return (alloc_map_[line_index] & (static_cast<MapLineType>(1) << bit_index)) != 0;
}

void BitmapMemoryManager::setBit(FrameID frame, bool allocated) {
    auto line_index = frame.ID() / kBitsPerMapLine;
    auto bit_index = frame.ID() % kBitsPerMapLine;

    Log(kDebug, "setBit: frame.ID=%d, line_index=%d, bit_index=%d\n", frame.ID(), line_index, bit_index);

    if (allocated) {
        alloc_map_[line_index] |= (static_cast<MapLineType>(1) << bit_index);
    } else {
        alloc_map_[line_index] &= ~(static_cast<MapLineType>(1) << bit_index);
    }
}

extern "C" caddr_t program_break, program_break_end;

Error InitializeHeap(BitmapMemoryManager& memory_manager) {
    const int kHeapFrames = 64 * 512;
    const auto heap_start = memory_manager.Allocate(kHeapFrames);
    if (heap_start.error) {
        return heap_start.error;
    }

    program_break = reinterpret_cast<caddr_t>(heap_start.value.ID() * kBytesPerFrame);
    program_break_end = program_break + kHeapFrames * kBytesPerFrame;
    return MAKE_ERROR(Error::kSuccess);
}
