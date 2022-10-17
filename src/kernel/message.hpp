#pragma once

enum class LayerOperation {
    Move,
    MoveRelative,
    Draw,
    DrawArea,
};

struct Message {
    enum Type {
        kInterruptXHCI,
        kInterruptLAPICTimer,
        kTimerTimeout,
        kKeyPush,
        kLayer,
        kLayerFinish,
        kMouseMove,
        kMouseButton,
        kWindowActive,
    } type;

    uint64_t src_task;

    union {
        struct {
            unsigned long timeout;
            int value;
        } timer;

        struct {
            uint8_t modifier;
            uint8_t keycode;
            char ascii;
            int press;
        } keyboard;

        struct  {
            LayerOperation op;
            unsigned int layer_id;
            int x, y;
            int w, h;
        } layer;

        struct {
            int x, y;
            int dx, dy;
            uint8_t buttons;
        } mouse_move;

        struct {
            int x, y;
            int press;
            int button;
        } mouse_button;

        struct {
            int activate;
        } window_active;
    } arg;
};
